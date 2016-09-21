#include        "includes.h"
#include        "offsets.h"

/* Forward*/
static void filltypecodes(void);
static void processenums(void);
static void processtypes(void);
static void processcompound(void);
static void processvars(void);
static void processattributes(void);

static void processdatalists(void);
static void processdatalist(Symbol*);
static void processdatalist1(Symbol*, Datasrc*);
static void processfieldarray(Symbol*, Datasrc*, Dimset*, int);
static void processvardata(Symbol*,Datalist*);
static void processvardata1(Symbol*,Datasrc*,Odometer*,int);
static void processfillvalue(Symbol*, Datasrc*);

static void inferattributetype(Symbol* asym);
static void checkconsistency(void);
static void validate(void);
static int tagvlentypes(Symbol* tsym);


static Symbol* grouptreelocate(Symbol* refsym, Symbol* root, Symbol* lastgroup);

List* vlenconstants;  /* List<Constant*>;*/
			  /* ptr to vlen instances across all datalists*/

/* Post-parse semantic checks and actions*/
void
processsemantics(void)
{
    /* Process each type and sort by dependency order*/
    processtypes();
    /* Make sure all typecodes are set if basetype is set*/
    filltypecodes();
    /* Process each var to fill in missing fields, etc*/
    processvars();
    /* Process attributes to connect to corresponding variable*/
    processattributes();
    /* Process each compound to compute its size*/
    processcompound();
    /* Fix up enum constant values*/
    processenums();
    /* Fix up datalists*/
    processdatalists();
    /* check internal consistency*/
    checkconsistency();
    /* do any needed additional semantic checks*/
    validate();
}

/* Given a reference symbol, produce the corresponding*/
/* definition symbol; return NULL if there is no definition*/
/* Note that this is somewhat complicated to conform to*/
/* various scoping rules, namely:*/
/* 1. look into parent hierarchy for un-prefixed dimension names.*/
/* 2. look in whole group tree for un-prefixed type names;*/
/*    search is breadth first.*/
/* 3. look in the same group as ref for un-prefixed variable names.*/
/* 4. ditto for group references*/
/* 5. look in whole group tree for un-prefixed enum constants*/

Symbol*
locate(Symbol* refsym)
{
    Symbol* sym = NULL;
    switch (refsym->objectclass) {
    case NC_DIM:
	if(refsym->is_prefixed) {
	    /* locate exact dimension specified*/
	    sym = lookup(NC_DIM,refsym);
	} else { /* Search for matching dimension in all parent groups*/
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
	    while(parent != NULL) {
		/* search this parent for matching name and type*/
		sym = lookupingroup(NC_DIM,refsym->name,parent);
		if(sym != NULL) break;
		parent = parent->container;
	    }
	}		
	break;
    case NC_TYPE:
	if(refsym->is_prefixed) {
	    /* locate exact type specified*/
	    sym = lookup(NC_TYPE,refsym);
	} else {
	    int i; /* Search for matching type in all groups (except...)*/
	    /* Short circuit test for primitive types*/
	    for(i=NC_NAT;i<=NC_STRING;i++) {
		Symbol* prim = basetypefor(i);
		if(prim == NULL) continue;
	        if(strcmp(refsym->name,prim->name)==0) {
		    sym = prim;
		    break;
		}
	    }
	    if(sym != NULL) break;
	    sym = grouptreelocate(refsym,rootgroup,refsym->location);
	}		
	break;
    case NC_VAR:
	if(refsym->is_prefixed) {
	    /* locate exact variable specified*/
	    sym = lookup(NC_VAR,refsym);
	} else {
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
   	    /* search this parent for matching name and type*/
	    sym = lookupingroup(NC_VAR,refsym->name,parent);
	}		
        break;
    case NC_GRP:
	if(refsym->is_prefixed) {
	    /* locate exact group specified*/
	    sym = lookup(NC_GRP,refsym);
	} else {
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
   	    /* search this parent for matching name and type*/
	    sym = lookupingroup(NC_GRP,refsym->name,parent);
	}		
	break;

    default: PANIC1("locate: bad refsym type: %d",refsym->objectclass);
    }
    if(debug > 1) {
	char* ncname;
	if(refsym->objectclass == NC_TYPE)
	    ncname = ncclassname(refsym->subclass);
	else
	    ncname = ncclassname(refsym->objectclass);
	fdebug("locate: %s: %s -> %s\n",
		ncname,fullname(refsym),(sym?fullname(sym):"NULL"));
    }   
    return sym;
}

/*
Search for an object in all groups using preorder depth-first traversal.
stop after searching (at top level) the lastgroup.
*/
static Symbol*
grouptreelocate(Symbol* refsym, Symbol* root, Symbol* lastgroup)
{
    int i;
    Symbol* sym = NULL;
    /* search the root for matching name and major type*/
    sym = lookupingroup(refsym->objectclass,refsym->name,root);
    if(sym == NULL && root != lastgroup) {
	for(i=0;i<listlength(root->subnodes);i++) {
	    Symbol* grp = (Symbol*)listget(root->subnodes,i);
	    if(grp->objectclass == NC_GRP && !grp->is_ref) {
		sym = grouptreelocate(refsym,grp,lastgroup);
	    }
	    if(sym != NULL) break;
	}
    }
    return sym;
}


/* 1. Do a topological sort of the types based on dependency*/
/*    so that the least dependent are first in the typdefs list*/
/* 2. fill in type typecodes*/
/* 3. mark types that use vlen*/
static void
processtypes(void)
{
    int i,j,keep,added;
    List* sorted = listnew(); /* hold re-ordered type set*/
    /* Prime the walk by capturing the set*/
    /*     of types that are dependent on primitive types*/
    /*     e.g. uint vlen(*) or primitive types*/
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* sym = (Symbol*)listget(typdefs,i);
	keep=0;
	switch (sym->subclass) {
	case NC_PRIM: /*ignore pre-defined primitive types*/
	    sym->touched=1;
	    break;
	case NC_OPAQUE:
	case NC_ENUM:
	    keep=1;
	    break;
        case NC_VLEN: /* keep if its basetype is primitive*/
	    if(sym->typ.basetype->subclass == NC_PRIM) keep=1;
	    break;	    	
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    keep=1; /*assume all fields are primitive*/
	    for(j=0;j<listlength(sym->subnodes);j++) {
		Symbol* field = (Symbol*)listget(sym->subnodes,j);
		assert(field->subclass == NC_FIELD);
		if(field->typ.basetype->subclass != NC_PRIM) {keep=0;break;}
	    }	  
	    break;
	default: break;/* ignore*/
	}
	if(keep) {
	    sym->touched = 1;
	    listpush(sorted,(elem_t)sym);
	}
    }	
    /* 2. repeated walk to collect level i types*/
    do {
        added=0;
        for(i=0;i<listlength(typdefs);i++) {
	    Symbol* sym = (Symbol*)listget(typdefs,i);
	    if(sym->touched) continue; /* ignore already processed types*/
	    keep=0; /* assume not addable yet.*/
	    switch (sym->subclass) {
	    case NC_PRIM: 
	    case NC_OPAQUE:
	    case NC_ENUM:
		PANIC("type re-touched"); /* should never happen*/
	        break;
            case NC_VLEN: /* keep if its basetype is already processed*/
	        if(sym->typ.basetype->touched) keep=1;
	        break;	    	
	    case NC_COMPOUND: /* keep if all fields are processed*/
	        keep=1; /*assume all fields are touched*/
	        for(j=0;j<listlength(sym->subnodes);j++) {
		    Symbol* field = (Symbol*)listget(sym->subnodes,j);
		    assert(field->subclass == NC_FIELD);
		    if(!field->typ.basetype->touched) {keep=1;break;}
	        }	  
	        break;
	    default: break;				
	    }
	    if(keep) {
		listpush(sorted,(elem_t)sym);
		sym->touched = 1;
		added++;
	    }	    
	}
    } while(added > 0);
    /* Any untouched type => circular dependency*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	if(tsym->touched) continue;
	semerror(tsym->lineno,"Circular type dependency for type: %s",fullname(tsym));
    }
    listfree(typdefs);
    typdefs = sorted;
    /* fill in typecodes*/
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* sym = (Symbol*)listget(typdefs,i);
	if(sym->typ.basetype != NULL && sym->typ.typecode == NC_NAT)
	    sym->typ.typecode = sym->typ.basetype->typ.typecode;
    }
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* tsym = (Symbol*)listget(typdefs,i);
	tagvlentypes(tsym);
    }
}

/* Recursively check for vlens*/
static int
tagvlentypes(Symbol* tsym)
{
    int tagged = 0;
    int j;
    switch (tsym->subclass) {
        case NC_VLEN: 
	    tagged = 1;
	    tagvlentypes(tsym->typ.basetype);
	    break;	    	
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    for(j=0;j<listlength(tsym->subnodes);j++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,j);
		assert(field->subclass == NC_FIELD);
		if(tagvlentypes(field->typ.basetype)) tagged = 1;
	    }	  
	    break;
	default: break;/* ignore*/
    }
    if(tagged) tsym->typ.hasvlen = 1;
    return tagged;
}

/* Make sure all typecodes are set if basetype is set*/
static void
filltypecodes(void)
{
    Symbol* sym;
    for(sym=symlist;sym != NULL;sym = sym->next) {    
	if(sym->typ.basetype != NULL && sym->typ.typecode == NC_NAT)
	    sym->typ.typecode = sym->typ.basetype->typ.typecode;
    }
}

static void
processenums(void)
{
    int i,j;
    List* enumids = listnew();
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* sym = (Symbol*)listget(typdefs,i);
	assert(sym->objectclass == NC_TYPE);
	if(sym->subclass != NC_ENUM) continue;
	for(j=0;j<listlength(sym->subnodes);j++) {
	    Symbol* esym = (Symbol*)listget(sym->subnodes,j);
	    assert(esym->subclass == NC_ECONST);
	    listpush(enumids,(elem_t)esym);
	}
    }	    
    /* Now walk set of enum ids to look for duplicates with same prefix*/
    for(i=0;i<listlength(enumids);i++) {
	Symbol* sym1 = (Symbol*)listget(enumids,i);
        for(j=i+1;j<listlength(enumids);j++) {
	   Symbol* sym2 = (Symbol*)listget(enumids,j);
	   if(strcmp(sym1->name,sym2->name) != 0) continue;
	   if(!prefixeq(sym1->prefix,sym2->prefix)) continue;
	   semerror(sym1->lineno,"Duplicate enumeration ids in same scope: %s",
		   fullname(sym1));	
	}
    }    
    /* Convert enum values to match enum type*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	assert(tsym->objectclass == NC_TYPE);
	if(tsym->subclass != NC_ENUM) continue;
	for(j=0;j<listlength(tsym->subnodes);j++) {
	    Symbol* esym = (Symbol*)listget(tsym->subnodes,j);
	    Constant newec;
	    assert(esym->subclass == NC_ECONST);
	    newec.nctype = esym->typ.typecode;
	    convert1(&esym->typ.econst,&newec);
	    esym->typ.econst = newec;
	}	
    }
}

/* Compute compound sizes and offsets*/
void
computesize(Symbol* tsym)
{
    int i;
    int offset = 0;
    unsigned long totaldimsize;
    if(tsym->touched) return;
    tsym->touched=1;
    switch (tsym->subclass) {
        case NC_VLEN: /* actually two sizes for vlen*/
	    computesize(tsym->typ.basetype); /* first size*/
	    tsym->typ.size = ncsize(tsym->typ.typecode);
	    tsym->typ.alignment = nctypealignment(tsym->typ.typecode);
	    break;
	case NC_PRIM:
	    tsym->typ.size = ncsize(tsym->typ.typecode);
	    tsym->typ.alignment = nctypealignment(tsym->typ.typecode);
	    break;
	case NC_OPAQUE:
	    /* size and alignment already assigned*/
	    break;
	case NC_ENUM:
	    computesize(tsym->typ.basetype); /* first size*/
	    tsym->typ.size = tsym->typ.basetype->typ.size;
	    tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	    break;
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    /* First, compute recursively, the size and alignment of fields*/
	    for(i=0;i<listlength(tsym->subnodes);i++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,i);
		assert(field->subclass == NC_FIELD);
		computesize(field);
		/* alignment of struct is same as alignment of first field*/
		if(i==0) tsym->typ.alignment = field->typ.alignment;
	    }	  
	    /* now compute the size of the compound based on*/
	    /* what user specified*/
	    offset = 0;
	    for(i=0;i<listlength(tsym->subnodes);i++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,i);
		/* only support 'c' alignment for now*/
		int alignment = field->typ.alignment;
		offset += getpadding(offset,alignment);
		field->typ.offset = offset;
		offset += field->typ.size;
	    }
	    tsym->typ.size = offset;
	    break;
        case NC_FIELD: /* Compute size of all non-unlimited dimensions*/
	    if(tsym->typ.dimset.ndims > 0) {
	        computesize(tsym->typ.basetype);
	        totaldimsize = arraylength(&tsym->typ.dimset);
	        tsym->typ.size = tsym->typ.basetype->typ.size * totaldimsize;
	        tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	    } else {
	        tsym->typ.size = tsym->typ.basetype->typ.size;
	        tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	    }
	    break;
	default:
	    PANIC1("computesize: unexpected type class: %d",tsym->subclass);
	    break;
    }
}

void
processvars(void)
{
    int i,j;
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	Symbol* tsym = vsym->typ.basetype;
	/* fill in the typecode*/
	vsym->typ.typecode = tsym->typ.typecode;
	for(j=0;j<tsym->typ.dimset.ndims;j++) {
	    /* deref the dimensions*/
	    tsym->typ.dimset.dimsyms[j] = tsym->typ.dimset.dimsyms[j];
#ifndef USE_NETCDF4
	    /* UNLIMITED must only be in first place*/
	    if(tsym->typ.dimset.dimsyms[j]->dim.size == NC_UNLIMITED) {
		if(j != 0)
		    semerror(vsym->lineno,"Variable: %s: UNLIMITED must be in first dimension only",fullname(vsym));
	    }
#endif
	}	
    }
}

void
processcompound(void)
{
    int i;
    /* use touch flag to avoid circularity*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	tsym->touched = 0;
    }
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	computesize(tsym); /* this will recurse*/
    }
}

void
processattributes(void)
{
    int i,j;
    /* process global attributes*/
    for(i=0;i<listlength(gattdefs);i++) {
	Symbol* asym = (Symbol*)listget(gattdefs,i);
	if(asym->typ.basetype == NULL) inferattributetype(asym);
	/* fill in the typecode*/
	asym->typ.typecode = asym->typ.basetype->typ.typecode;
    }
    /* process per variable attributes*/
    for(i=0;i<listlength(attdefs);i++) {
	Symbol* asym = (Symbol*)listget(attdefs,i);
	if(asym->typ.basetype == NULL) inferattributetype(asym);
	/* fill in the typecode*/
	asym->typ.typecode = asym->typ.basetype->typ.typecode;
    }
    /* collect per-variable attributes per variable*/
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	List* list = listnew();
        for(j=0;j<listlength(attdefs);j++) {
	    Symbol* asym = (Symbol*)listget(attdefs,j);
	    assert(asym->att.var != NULL);
	    if(asym->att.var != vsym) continue;	    
            listpush(list,(elem_t)asym);
	}
	vsym->var.attributes = list;
    }
}

/* Look at the first primitive value of the*/
/* attribute's datalist to infer the type of the attribute.*/
/* There is a potential ambiguity when that value is a string.*/
/* Is the attribute type NC_CHAR or NC_STRING?*/
/* The answer is we always assume it is NC_STRING.*/
/* When later constructing the .nc file, and when using*/
/* classic, this will be automatically downgraded to NC_CHAR.*/

static nc_type
inferattributetype1(Datasrc* src)
{
    nc_type result = NC_NAT;
    /* Recurse down any enclosing compound markers to find first non-fill "primitive"*/
    while(result == NC_NAT && srcmore(src)) {
	if(issublist(src)) {
	    srcpush(src);
	    result = inferattributetype1(src);
	    srcpop(src);
	} else {	
	    Constant* con = srcnext(src);
	    if(isprimplus(con->nctype)) result = con->nctype;
	    /* else keep looking*/
	}
    }
    return result;
}

static void
inferattributetype(Symbol* asym)
{
    Datalist* datalist;
    Datasrc src;
    nc_type nctype;
    assert(asym->data != NULL);
    datalist = asym->data;
    assert(datalist != NULL && datalist->length > 0);
    src = makedatasrc(datalist);
    nctype = inferattributetype1(&src);    
    /* get the corresponding primitive type built-in symbol*/
    /* special case for string*/
    if(usingclassic && nctype == NC_STRING)
        asym->typ.basetype = basetypefor(NC_CHAR);
    else
	asym->typ.basetype = basetypefor(nctype);
}

/* Find name within group structure*/
Symbol*
lookupgroup(List* prefix)
{
#ifdef USE_NETCDF4
    if(prefix == NULL || listlength(prefix) == 0)
	return rootgroup;
    else
	return (Symbol*)listtop(prefix);
#else
    return rootgroup;
#endif
}

/* Find name within given group*/
Symbol*
lookupingroup(nc_class objectclass, char* name, Symbol* grp)
{
    int i;
    if(name == NULL) return NULL;
    if(grp == NULL) grp = rootgroup;
dumpgroup(grp);
    for(i=0;i<listlength(grp->subnodes);i++) {
	Symbol* sym = (Symbol*)listget(grp->subnodes,i);
	if(sym->is_ref) continue;
	if(sym->objectclass != objectclass) continue;
	if(strcmp(sym->name,name)!=0) continue;
	return sym;
    }
    return NULL;
}

/* Find symbol within group structure*/
Symbol*
lookup(nc_class objectclass, Symbol* pattern)
{
    Symbol* grp;
    if(pattern == NULL) return NULL;
    grp = lookupgroup(pattern->prefix);
    if(grp == NULL) return NULL;
    return lookupingroup(objectclass,pattern->name,grp);
}

#ifndef NO_STDARG
void
semerror(const int lno, const char *fmt, ...)
#else
void
semerror(lno,fmt,va_alist) const int lno; const char* fmt; va_dcl
#endif
{
    va_list argv;
    vastart(argv,fmt);
    (void)fprintf(stderr,"%s: %s line %d: ", progname, cdlname, lno);
    vderror(fmt,argv);
    exit(1);
}


/* return internal size for values of specified netCDF type */
size_t
nctypesize(
     nc_type type)			/* netCDF type code */
{
    switch (type) {
      case NC_BYTE: return sizeof(char);
      case NC_CHAR: return sizeof(char);
      case NC_SHORT: return sizeof(short);
      case NC_INT: return sizeof(int);
      case NC_FLOAT: return sizeof(float);
      case NC_DOUBLE: return sizeof(double);
      case NC_UBYTE: return sizeof(unsigned char);
      case NC_USHORT: return sizeof(unsigned short);
      case NC_UINT: return sizeof(unsigned int);
      case NC_INT64: return sizeof(long long);
      case NC_UINT64: return sizeof(unsigned long long);
      case NC_STRING: return sizeof(char*);
      default:
	PANIC("nctypesize: bad type code");
    }
    return 0;
}

static int
sqContains(List* seq, Symbol* sym)
{
    int i;
    if(seq == NULL) return 0;
    for(i=0;i<listlength(seq);i++) {
        Symbol* sub = (Symbol*)listget(seq,i);
	if(sub == sym) return 1;
    }
    return 0;
}

static void
checkconsistency(void)
{
    int i;
    for(i=0;i<listlength(grpdefs);i++) {
	Symbol* sym = (Symbol*)listget(grpdefs,i);
	if(sym == rootgroup) {
	    if(sym->container != NULL)
	        PANIC("rootgroup has a container");
	} else if(sym->container == NULL && sym != rootgroup)
	    PANIC1("symbol with no container: %s",sym->name);
	else if(sym->container->is_ref != 0)
	    PANIC1("group with reference container: %s",sym->name);
	else if(sym != rootgroup && !sqContains(sym->container->subnodes,sym))
	    PANIC1("group not in container: %s",sym->name);
	if(sym->subnodes == NULL)
	    PANIC1("group with null subnodes: %s",sym->name);
    }
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* sym = (Symbol*)listget(typdefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("type not in container: %s",sym->name);
    }
    for(i=0;i<listlength(dimdefs);i++) {
	Symbol* sym = (Symbol*)listget(dimdefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("dimension not in container: %s",sym->name);
    }
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* sym = (Symbol*)listget(vardefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("variable not in container: %s",sym->name);
	if(!(isprimplus(sym->typ.typecode)
	     || sqContains(typdefs,sym->typ.basetype)))
	    PANIC1("variable with undefined type: %s",sym->name);
    }
}

static void
validate(void)
{
    int i;
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* sym = (Symbol*)listget(vardefs,i);
	if(sym->var.special._Fillvalue != NULL) {
	}
    }
}

/*
Do any pre-processing of datalists.
1. Compute the effective size of unlimited
   dimensions vis-a-vis this data list
2. Compute the length of attribute lists
3. Collect the VLEN constants
*/

void
processdatalists(void)
{
    int i;
    if(debug > 0) fdebug("processdatalists:\n");
    vlenconstants = listnew();
    listsetalloc(vlenconstants,1024);
    /* process global attributes*/
    for(i=0;i<listlength(gattdefs);i++) {
	Symbol* asym = (Symbol*)listget(gattdefs,i);
	processdatalist(asym);
        if(debug > 0 && asym->data != NULL) {
	    fdebug(":%s.datalist: ",asym->name);
	    dumpdatalist(asym->data,"");
	    fdebug("\n");
	}
    }
    /* process per variable attributes*/
    for(i=0;i<listlength(attdefs);i++) {
	Symbol* asym = (Symbol*)listget(attdefs,i);
	processdatalist(asym);
        if(debug > 0 && asym->data != NULL) {
	    fdebug("%s:%s.datalist: ",asym->att.var->name,asym->name);
	    dumpdatalist(asym->data,"");
	    fdebug("\n");
	}
    }
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	processdatalist(vsym);
        if(debug > 0 && vsym->data != NULL) {
	    fdebug("%s.datalist: ",vsym->name);
	    dumpdatalist(vsym->data,"");
	    fdebug("\n");
	}
    }
}

static void
processdatalist(Symbol* sym)
{
    Datasrc src;
    if(sym->data == NULL) return;
    src = makedatasrc(sym->data);
    if(sym->objectclass == NC_VAR) {
	if(sym->typ.dimset.ndims > 0) {
            processvardata(sym,sym->data);
	} else
	    processdatalist1(sym->typ.basetype,&src);
    } else if(sym->objectclass == NC_ATT) {
	int count = 0;
	/* treat as if it had an implicit NC_UNLIMITED dimension*/
	while(srcmore(&src)) {
	    processdatalist1(sym->typ.basetype,&src);
	    count++;
	}
	sym->att.count = count;
    } else if(sym->objectclass == NC_TYPE) {/* Never occurs for now*/
        processdatalist1(sym,&src);
    }
}

static void
processdatalist1(Symbol* sym, Datasrc* src)
{
    int i,iscmpd,count;
    Constant* cmpd;

    cmpd = srcpeek(src);

    if(cmpd == NULL || cmpd->nctype == NC_FILLVALUE) {
	srcnext(src);
	processfillvalue(sym,src);
	return;
    }

    switch (sym->subclass) {

    case NC_ENUM: case NC_OPAQUE: case NC_PRIM: 
	srcnext(src);
	break;

    case NC_COMPOUND:
        SRCPUSH(iscmpd,src);
	for(i=0;i<listlength(sym->subnodes) && srcmore(src);i++) {
	    Symbol* field = (Symbol*)listget(sym->subnodes,i);
	    processdatalist1(field,src);
	}
        SRCPOP(iscmpd,src);
	srcnext(src);
	break;
    case NC_VLEN:
        if(!issublist(src)) {/* fail on no compound*/
           semerror(srcline(src),"Vlen constants must be enclosed in {..}");
        }
	cmpd = srcnext(src);
	srcpushlist(src,cmpd->value.compoundv); /* enter the sublist*/
	count = 0;
        while(srcmore(src)) {
            processdatalist1(sym->typ.basetype,src);
	    count++;
        }
        srcpop(src);
	cmpd->value.compoundv->vlen.count = count;	
	cmpd->value.compoundv->vlen.uid = listlength(vlenconstants);
	cmpd->value.compoundv->schema = sym;
	listpush(vlenconstants,(elem_t)cmpd);
	break;

    case NC_FIELD:
        if(sym->typ.dimset.ndims > 0) {
	    processfieldarray(sym->typ.basetype,src,&sym->typ.dimset,0);
	} else
	    processdatalist1(sym->typ.basetype,src);
	break;

    default: PANIC1("processdatalist: unexpected subclass %d",sym->subclass);
    }
}

/* Used only for structure field arrays*/
static void
processfieldarray(Symbol* basetype, Datasrc* src, Dimset* dimset, int index)
{
    int i,iscmpd;
    Symbol* dim = dimset->dimsyms[index];
    unsigned int size = dim->dim.size;
    int lastdim = (index == (dimset->ndims - 1)); /* last dimension*/

    assert(size != 0);
    SRCPUSH(iscmpd,src);
    if(lastdim) {
        if(basetype->typ.typecode == NC_CHAR) {
	    /* If we are trying to get a matrix of characters, then*/
	    /* special handling has to be applied in case the characters*/
	    /* are embedded in string constants.*/
	    gen_string(size,src,NULL);
	} else {
	    for(i=0;i<size;i++) {
	        processdatalist1(basetype,src);
	    }
	}
    } else { /* !lastdim*/
	for(i=0;i<size;i++) {
	    processfieldarray(basetype,src,dimset,index+1);
	}
    }
    SRCPOP(iscmpd,src);
}

static void
processfillvalue(Symbol* sym, Datasrc* src)
{
    Datalist* list = NULL;
    assert(sym->objectclass == NC_TYPE);
    list = getfiller(sym);
    srcpushlist(src,list);
    processdatalist1(sym,src);
    srcpop(src);
}

static void
processvardata(Symbol* var, Datalist* data)
{
    Odometer* odom = newodometer(&var->typ.dimset);
    Datasrc src = makedatasrc(data);
    processvardata1(var,&src,odom,0);
    data->dimdata = odom;
}

static void
processvardata1(Symbol* var, Datasrc* src, Odometer* odom, int index)
{
    int i,iscmpd;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int firstdim = (index == 0);
    int declsize = odom->dims[index].declsize;
    int isunlimited = (declsize == 0);
    Symbol* basetype = var->typ.basetype;
    unsigned long count;

    assert(index >= 0 && index < rank);

    count = 0;

    if(!firstdim && isunlimited && !issublist(src)) {
	semerror(srcline(src),"UNLIMITED dimension constants (other than top level) must be enclosed in {..} or \"..\"");
    }

    if(!firstdim ) SRCPUSH(iscmpd,src);
    if(lastdim) { /* Might also be firstdim*/
        if(basetype->typ.typecode == NC_CHAR) {
            Constant str = gen_string((isunlimited?0:declsize),src,NULL);
    	    count = str.value.stringv.len;
        } else {
	    if(isunlimited) {
                while((srcpeek(src))!=NULL) {
                    processdatalist1(basetype,src);
	    	    count++;
	        }
	    } else for(i=0;i<declsize;i++) {
                processdatalist1(basetype,src);
		count++;
	    }
        }
    } else {/* all other cases*/
	if(isunlimited) {
            while(srcmore(src)) {
                processvardata1(var,src,odom,index+1);
		count++;
            }
	} else for(i=0;i<declsize;i++) {
            processvardata1(var,src,odom,index+1);
	    count++;
	}
    }
    if(!firstdim) SRCPOP(iscmpd,src);
    /* Use max function to track datasize for unlimited cases*/
    if(isunlimited && (count > odom->dims[index].datasize)) {
        odom->dims[index].datasize = count;
    } else {/* verify*/
	if(count != odom->dims[index].declsize) {
	    semerror(srcline(src),"Dimension mismatch: %s[%d]: %lu :: %lu",
			var->name,index,count,odom->dims[index].declsize);
	}
        odom->dims[index].datasize = count;
    }
}
