#include "includes.h"

/**************************************************/
/* Code for generating C language data lists*/
/**************************************************/

extern List* vlenconstants;

/* Forward*/
static void genc_datalist1(Symbol* sym, Datasrc*,Symbol*,Bytebuffer*);
static void genc_vardata(Symbol*, Datalist*, Putvar*, Bytebuffer*);
static void genc_primdata(Symbol*, Datasrc*, Bytebuffer*);
static void genc_fillvalue(Symbol*, Symbol*, Datasrc*,Bytebuffer*);
static void genc_fielddata(Symbol*, Datasrc*, Dimset*, int, Bytebuffer*);
static void genc_vardata1(Symbol* basetype,
	      Datasrc* src,
	      Odometer* odom,
	      Putvar* closure,
	      int index,
	      int checkpoint,
	      Bytebuffer* code);

/*
Possible Optimizations:
1. don't use convert1, but rely on c compiler to do conversions
*/

/*
Datalist rules:
1. the top level is automatically assumed to be a list of items,
   so it should not be inside {...}.
2. Instances of vlens or UNLIMITED dimensions (other than the first dimension)
   must be surrounded by {...} in order to specify the size of the vlen or UNLIMITED.
3. Bounded dimensions may optionally be embedded in {...}.
4. Compound instances may optionally be embedded in {...}
5. Strings may be interpreted as sequences of characters if the basetype is NC_CHAR.
*/

void
genc_datalist(Symbol* tvsym, Datalist* list, Putvar* closure, Bytebuffer* databuf)
{
    if(tvsym->objectclass == NC_VAR && tvsym->typ.dimset.ndims > 0) {
        genc_vardata(tvsym,list,closure,databuf);
    } else {
        Datasrc src = makedatasrc(list);
	switch (tvsym->objectclass) {
	case NC_VAR:
	    genc_datalist1(tvsym->typ.basetype,&src,tvsym,databuf);
	    break;
	case NC_ATT: {
	    int first = 1;
	    while(srcmore(&src)) {
	        if(!first) bbCat(databuf,", "); else first = 0;
	        genc_datalist1(tvsym->typ.basetype,&src,NULL,databuf);
	    }
	    } break;
	default:
            genc_datalist1(tvsym,&src,NULL,databuf);
	    break;
	}
        if(srcmore(&src)) {
	    semerror(srcline(&src),"Extra data at end of datalist");
        }
    }
}

void
genc_datalist1(Symbol* tvsym, Datasrc* datasrc, Symbol* fillsrc, Bytebuffer* databuf)
{
    Constant* con = srcpeek(datasrc);
    if(con == NULL || con->nctype == NC_FILLVALUE) {
	srcnext(datasrc);
	genc_fillvalue(tvsym,fillsrc,datasrc,databuf);
	return;
    }

    switch (tvsym->subclass) {

    case NC_ENUM:
    case NC_OPAQUE:
    case NC_PRIM:
	if(issublist(datasrc)) {
	    semerror(srcline(datasrc),"Expected primitive found {..}");
	    srcpush(datasrc);
	    genc_primdata(tvsym,datasrc,databuf);
	    srcpop(datasrc);
	}
	genc_primdata(tvsym,datasrc,databuf);
	break;

    case NC_COMPOUND: {
	int i,iscmpd;
        bbCat(databuf,"{");
        SRCPUSH(iscmpd,datasrc); /* {..} is optional*/
        for(i=0;i<listlength(tvsym->subnodes);i++) {
            Symbol* field = (Symbol*)listget(tvsym->subnodes,i);
            if(i > 0) bbCat(databuf,", ");
	    if(!srcmore(datasrc)) { /* generate a fill value*/
	        Datalist* filllist = getfiller(tvsym);
	        Datasrc fillsrc = makedatasrc(filllist);
	        genc_datalist1(field,&fillsrc,NULL,databuf);
	    } else
	        genc_datalist1(field,datasrc,NULL,databuf);
	}
        SRCPOP(iscmpd,datasrc);
        bbCat(databuf,"}");
	} break;

    case NC_VLEN: {
	char stmt[C_MAX_STMT];
        Constant* cp;
        if(!issublist(datasrc)) {
	    semerror(srcline(datasrc),"Vlen data must be enclosed in {..}");
        }
        cp = srcnext(datasrc);
        /* generate the nc_vlen_t instance*/
        sprintf(stmt,"{%u, (void*)vlen_%u}",
	         cp->value.compoundv->vlen.count,
    		 cp->value.compoundv->vlen.uid);
        bbCat(databuf,stmt);
        } break;

    case NC_FIELD:
	if(tvsym->typ.dimset.ndims > 0) {
            genc_fielddata(tvsym->typ.basetype,datasrc,&tvsym->typ.dimset,0,databuf);
	} else
	    genc_datalist1(tvsym->typ.basetype,datasrc,NULL,databuf);
	break;

    default: PANIC1("genc_datalist: unexpected subclass %d",tvsym->subclass);
    }
}

/* Used only for structure field arrays*/
static void
genc_fielddata(Symbol* basetype, Datasrc* src, Dimset* dimset, int index, Bytebuffer* databuf)
{
    int i,iscmpd;
    Symbol* dim = dimset->dimsyms[index];
    unsigned int size = dim->dim.size;
    int lastdim = (index == (dimset->ndims - 1)); /* last dimension*/

    assert(size != 0);

    bbCat(databuf,"{");

    SRCPUSH(iscmpd,src);
    if(lastdim) {
        if(basetype->typ.typecode == NC_CHAR) {
	    /* If we are trying to get a matrix of characters, then*/
	    /* special handling has to be applied in case the characters*/
	    /* are embedded in string constants.*/
	    Constant str = gen_string(size,src,NULL);
	    bbCat(databuf,cconst(&str));
	} else {
	    for(i=0;i<size;i++) {
	  	if(i > 0) bbCat(databuf,", ");
	        genc_datalist1(basetype,src,NULL,databuf);
	    }
	}
    } else { /* !lastdim*/
	for(i=0;i<size;i++) {
	    if(i > 0) bbCat(databuf,", ");
	    genc_fielddata(basetype,src,dimset,index+1,databuf);
	}
    }
    SRCPOP(iscmpd,src);
    bbCat(databuf,"}");
}

static void
genc_primdata(Symbol* tsym, Datasrc* src, Bytebuffer* databuf)
{
    Constant target, *prim;

    prim = srcpeek(src);
    assert(prim->nctype != NC_COMPOUND);
    target.nctype = tsym->typ.typecode;

    /* Special case string -> char conversion*/
    if(prim->nctype == NC_STRING && target.nctype == NC_CHAR) {
        Bytebuffer* memory = bbNew();
	Constant str = gen_string(1,src,memory);
	bbFree(memory);
	/* convert to a single char*/
	convert1(&str,&target);
	bbCat(databuf,cconst(&target));
	return;
    }

    prim = srcnext(src);
    if(prim == NULL) {
#ifdef GENFILL
        /* generate a fill value*/
	nc_getfill(&target);
	/* fall thru*/
#else
	return;
#endif
    }

    switch (target.nctype) {
        case NC_ECONST:
	    if(tsym->subclass != NC_ENUM) {
	        semerror(prim->lineno,"Conversion to enum not supported (yet)");
	        nc_getfill(&target);
	    } else {
	        target.nctype = NC_ECONST;
		convert1(prim,&target);
	    }
   	    break;
        case NC_OPAQUE:
	    convert1(prim,&target);
	    setprimlength(&target,tsym->typ.size*2);
	    break;
        default:
	    convert1(prim,&target);
	    break;
    }
    bbCat(databuf,cconst(&target));
}

static void
genc_fillvalue(Symbol* tsym, Symbol* fillsrc, Datasrc* src, Bytebuffer* databuf)
{
    Datalist* list = NULL;

    assert(tsym->objectclass == NC_TYPE);
    if(fillsrc != NULL) list = fillsrc->var.special._Fillvalue;
    if(list == NULL) list = getfiller(tsym);
    srcpushlist(src,list);
    genc_datalist1(tsym,src,NULL,databuf);
    srcpop(src);
}

/* Result is a pool string or a constant => do not free*/
char*
cconst(Constant* ci)
{
    char tmp[64];
    tmp[0] = '\0';
    switch (ci->nctype) {
    case NC_CHAR:
	strcpy(tmp,"'");
	escapifychar(ci->value.charv,tmp+1,'\'');
	strcat(tmp,"'");
	break;
    case NC_BYTE:
	sprintf(tmp,"%hhd",ci->value.int8v);
	break;
    case NC_SHORT:
	sprintf(tmp,"%hd",ci->value.int16v);
	break;
    case NC_INT:
	sprintf(tmp,"%d",ci->value.int32v);
	break;
    case NC_FLOAT:
	sprintf(tmp,"%.8g",ci->value.floatv);
	break;
    case NC_DOUBLE:
	sprintf(tmp,"%.16g",ci->value.doublev);
	break;
    case NC_UBYTE:
	sprintf(tmp,"%hhu",ci->value.int8v);
	break;
    case NC_USHORT:
	sprintf(tmp,"%hu",ci->value.uint16v);
	break;
    case NC_UINT:
	sprintf(tmp,"%uU",ci->value.uint32v);
	break;
    case NC_INT64:
	sprintf(tmp,"%lldLL",ci->value.int64v);
	break;
    case NC_UINT64:
	sprintf(tmp,"%lluLLU",ci->value.uint64v);
	break;
    case NC_ECONST:
	sprintf(tmp,"%s",cname(ci->value.enumv));
	break;
    case NC_STRING: {
	char* escaped = escapify(ci->value.stringv.stringv,
				 '"',ci->value.stringv.len);
	char* result = poolalloc(1+2+strlen(escaped));
	strcpy(result,"\"");
	strcat(result,escaped);
	strcat(result,"\"");
	return result;
	} break;
    case NC_OPAQUE: {
	char* bstring;
	char* p;
	int bslen;
	bslen=(4*ci->value.opaquev.len);
	bstring = poolalloc(bslen+2+1);
	strcpy(bstring,"\"");
	p = ci->value.opaquev.stringv;
	while(*p) {
	    strcat(bstring,"\\x");
	    strncat(bstring,p,2);	    	    
	    p += 2;	
	}
	strcat(bstring,"\"");
	return bstring;
	} break;

    default: FPANIC("ncstype: bad type code: %d",ci->nctype);
    }
    return pooldup(tmp); /*except for NC_STRING and NC_OPAQUE*/
}

void
genc_vlenconstants(Bytebuffer* databuf)
{
    int i,count;
    Datasrc vlensrc;
    char tmp[C_MAX_STMT];

    for(i=0;i<listlength(vlenconstants);i++) {
	Constant* cmpd = (Constant*)listget(vlenconstants,i);
	Symbol* tsym = cmpd->value.compoundv->schema;
        assert(tsym != NULL);
        sprintf(tmp,"static const %s vlen_%u[] = {",
	        ctypename(tsym->typ.basetype),
                cmpd->value.compoundv->vlen.uid);
	bbCat(databuf,tmp);
	count = 0;
	vlensrc = makedatasrc(cmpd->value.compoundv);
	while(srcmore(&vlensrc)) {
	    if(count > 0) bbCat(databuf,", ");
            genc_datalist1(tsym->typ.basetype,&vlensrc,NULL,databuf);
	    count++;
        }
	assert(count == cmpd->value.compoundv->vlen.count);
	bbCat(databuf,"} ;\n");
    }
}

/*
Because of the use of a one-pass algorithm,
the rules for handling arrays in datalists are as follows:
If we are at a compound constant ({..}), then
it is assumed to enclose the current dimension
and all later dimensions.

Thus for a rank 2 dimension (non-toplevel), say (2,3),
we could accept any of the following
	{1,2,3,4,5,6}
	{{1,2,3},{4,5,6}}
	{1,2,3,{4,5,6}}
	{{1,2,3},4,5,6}
*/

static void
genc_vardata(Symbol* var, Datalist* data, Putvar* closure, Bytebuffer* databuf)
{
    Datasrc src;
    int i,checkpoint = 0;

    assert(data->dimdata != NULL);
    src = makedatasrc(data);
    for(i=0;i<var->typ.dimset.ndims;i++) {
	if(var->typ.dimset.dimsyms[i]->dim.size == NC_UNLIMITED)
	    checkpoint = i;
    }
    genc_vardata1(var,&src,data->dimdata,closure,0,checkpoint,databuf);
}

static void
genc_vardata1(Symbol* vsym,
	      Datasrc* src,
	      Odometer* odom,
	      Putvar* closure,
	      int index,
	      int checkpoint,
	      Bytebuffer* databuf)
{
    int i,iscmpd;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int firstdim = (index == 0);
    int useclosure = (closure != NULL);
    int atcheckpoint = (index == checkpoint);
    int declsize = odom->dims[index].declsize;
    int isunlimited = (declsize == 0);
    Symbol* basetype = vsym->typ.basetype;
    Symbol* fillsrc = vsym;
    Constant* con;

    assert(index >= 0 && index < rank);

    odom->dims[index].index = 0; /* reset*/

    if(!firstdim ) SRCPUSH(iscmpd,src);

    if(lastdim && basetype->typ.typecode == NC_CHAR) {
        /* If we are trying to get a matrix of characters, then*/
	/* special handling has to be applied in case the characters*/
	/* are embedded in string constants.*/
	Constant str = gen_string((isunlimited?0:declsize),src,NULL);
	bbCat(databuf,"{");
	bbCat(databuf,cconst(&str));
	bbCat(databuf,"}");
	odom->dims[index].index += str.value.stringv.len;
	if(atcheckpoint && useclosure) {
            closure->putvar(closure,odom,databuf);
	}
    } else if(isunlimited) {
	int doseparator = 0;
	bbCat(databuf,"{");
	while((srcpeek(src))!=NULL) {
	    if(doseparator) bbCat(databuf,", "); else doseparator = 1;
	    if(lastdim) {
                genc_datalist1(basetype,src,fillsrc,databuf);
	    } else {
                genc_vardata1(vsym,src,odom,closure,index+1,checkpoint,databuf);
	    }
	    odom->dims[index].index++;
	    if(atcheckpoint && useclosure) {
                closure->putvar(closure,odom,databuf);
		doseparator = 0;
	    }
	}
	bbCat(databuf,"}");
    } else { /* !isunlimited*/
	int doseparator = 0;
	if(!firstdim) bbCat(databuf,"{");
	for(i=0;i<declsize;i++) {
	    con = srcpeek(src);
            if(con == NULL) break; /* no point in going on*/
            if(doseparator) bbCat(databuf,", "); else doseparator = 1;
            if(lastdim) {
                genc_datalist1(basetype,src,fillsrc,databuf);
            } else { /* ! lastdim*/
               genc_vardata1(vsym,src,odom,closure,index+1,checkpoint,databuf);
            }
            odom->dims[index].index++;
            if(atcheckpoint && useclosure) {
                closure->putvar(closure,odom,databuf);
	        doseparator = 0; /* start over*/
		if(!firstdim) bbCat(databuf,"{");
            }
	}
	if(!firstdim) bbCat(databuf,"}");
    }
    if(!firstdim ) SRCPOP(iscmpd,src);
}

