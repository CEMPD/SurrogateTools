#include "includes.h"
#include "offsets.h"
#include "odom.h"

/**************************************************/
/* Code for generating binary data lists*/
/**************************************************/

struct Vlendata {
    char* data;
    unsigned long count;
} * vlendata;

extern List* vlenconstants;

/* Forward*/
static void gencdf_datalist1(Symbol* sym, Datasrc*,Bytebuffer*,Symbol*);
static void gencdf_primdata(Symbol* tsym, Datasrc* src,Bytebuffer*);
static void gencdf_fillvalue(Symbol* sym, Symbol*, Datasrc*,Bytebuffer*);
static void gencdf_vardata(Symbol*, Datalist*, Bytebuffer*, Putvar*);

static void alignbuffer(Constant* prim, Bytebuffer* buf);

/* Handle toplevel (i.e. variable) arrays separate from nested arrays*/
static void gencdf_vardata1(Symbol* var, Datasrc* src,
			    Bytebuffer* memory, Odometer* odom,
			    Putvar* closure, int index, int checkpoint);

static void gencdf_fielddata(Symbol* basetype, Datasrc* src,
			     Dimset* dimset, Bytebuffer* memory, int index);
		

/**************************************************/

void
gencdf_datalist(Symbol* tvsym, Datalist* list, Bytebuffer* memory, Putvar* closure)
{
    if(tvsym->objectclass == NC_VAR && tvsym->typ.dimset.ndims > 0) {
        gencdf_vardata(tvsym,list,memory,closure);
    } else {
        Datasrc src = makedatasrc(list);
	switch (tvsym->objectclass) {
	case NC_VAR:  /* scalar variable*/
	    gencdf_datalist1(tvsym->typ.basetype,&src,memory,tvsym);
	    break;
	case NC_ATT:
	    while(srcmore(&src))
		gencdf_datalist1(tvsym->typ.basetype,&src,memory,NULL);
	    break;
	default:
	    gencdf_datalist1(tvsym,&src,memory,NULL);
	    break;
	}
        if(srcmore(&src)) {
	    semerror(srcline(&src),"Extra data at end of datalist");
        }
    }
}

static void
gencdf_datalist1(Symbol* tvsym, Datasrc* datasrc, Bytebuffer* memory, Symbol* fillsrc)
{
    Constant* con = srcpeek(datasrc);
    if(con == NULL || con->nctype == NC_FILLVALUE) {
	srcnext(datasrc);
	gencdf_fillvalue(tvsym,fillsrc,datasrc,memory);	
	return;
    }

    switch (tvsym->subclass) {

    case NC_ENUM:
    case NC_OPAQUE:
    case NC_PRIM:
	if(issublist(datasrc)) {
	    semerror(srcline(datasrc),"Expected primitive found {..}");
	    srcpush(datasrc);
	    gencdf_primdata(tvsym,datasrc,memory);
	    srcpop(datasrc);
	}
	gencdf_primdata(tvsym,datasrc,memory);
	break;

    case NC_COMPOUND: {
        int i,iscmpd;
        SRCPUSH(iscmpd,datasrc); /* {..} is optional*/
        for(i=0;i<listlength(tvsym->subnodes);i++) {
            Symbol* field = (Symbol*)listget(tvsym->subnodes,i);
            if(!srcmore(datasrc)) { /* generate a fill value*/
                Datalist* filllist = getfiller(tvsym);
                Datasrc fillsrc = makedatasrc(filllist);
                gencdf_datalist1(field,&fillsrc,memory,NULL);
            } else
                gencdf_datalist1(field,datasrc,memory,NULL);
        }
        SRCPOP(iscmpd,datasrc);
	} break;

    case NC_VLEN: {
	Constant* cp;
        nc_vlen_t ptr;
        if(!issublist(datasrc)) {
            semerror(srcline(datasrc),"Vlen data must be enclosed in {..}");
        }
        cp = srcnext(datasrc);
        /* generate the nc_vlen_t instance*/
        ptr.p = vlendata[cp->value.compoundv->vlen.uid].data;
        ptr.len = vlendata[cp->value.compoundv->vlen.uid].count;
        bbAppendn(memory,(char*)&ptr,sizeof(ptr));
	} break;

    case NC_FIELD:
	if(tvsym->typ.dimset.ndims > 0) {
            gencdf_fielddata(tvsym->typ.basetype,datasrc,&tvsym->typ.dimset,memory,0);
	} else
	    gencdf_datalist1(tvsym->typ.basetype,datasrc,memory,NULL);
	break;

    default: PANIC1("gencdf_datalist: unexpected subclass %d",tvsym->subclass);
    }
}

/* Used only for structure field arrays*/
static void
gencdf_fielddata(Symbol* basetype,
		 Datasrc* src,
		 Dimset* dimset,
		 Bytebuffer* memory,
		 int index)
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
	    Constant str = gen_string(size,src,NULL);
	    bbAppendn(memory,str.value.stringv.stringv,str.value.stringv.len);
	} else {
	    for(i=0;i<size;i++) {
	        gencdf_datalist1(basetype,src,memory,NULL);
	    }
	}
    } else { /* !lastdim*/
	for(i=0;i<size;i++) {
	    gencdf_fielddata(basetype,src,dimset,memory,index+1);
	}
    }
    SRCPOP(iscmpd,src);
}

static void
gencdf_primdata(Symbol* tsym, Datasrc* src, Bytebuffer* memory)
{
    Constant target, *prim;
    size_t nctypesize;

    prim = srcpeek(src);
    assert(prim->nctype != NC_COMPOUND);
    target.nctype = tsym->typ.typecode;
    nctypesize = ncsize(target.nctype);

    /* Special case string -> char conversion*/
    if(prim->nctype == NC_STRING && target.nctype == NC_CHAR) {
	gen_string(1,src,memory);
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

    if(target.nctype != NC_ECONST) {
	convert1(prim,&target);
        alignbuffer(&target,memory);
    }

    /* make sure there is plenty of room*/
/*    if(!bbAvail(memory,nctypesize)) {*/
/*	bbSetalloc(memory,2*bbAlloc(memory));*/
/*    }*/

    switch (target.nctype) {
        case NC_ECONST:
	    if(tsym->subclass != NC_ENUM) {
	        semerror(prim->lineno,"Conversion to enum not supported (yet)");
	    } else {
		Datalist* econ = builddatalist(1);
		srcpushlist(src,econ);
		dlappend(econ,&prim->value.enumv->typ.econst);
	        gencdf_primdata(prim->value.enumv->typ.basetype,src,memory);
		srcpop(src);
	    }
   	    break;
        case NC_OPAQUE: {
	    unsigned char* bytes;
	    size_t len;
	    setprimlength(&target,tsym->typ.size*2);
	    bytes=makebytestring(target.value.opaquev.stringv,&len);
	    bbAppendn(memory,(void*)bytes,len);
	    } break;

        case NC_CHAR:
            bbAppendn(memory,&target.value.charv,sizeof(target.value.charv));
	    break;
        case NC_BYTE:
            bbAppendn(memory,(void*)&target.value.int8v,sizeof(target.value.int8v));
	    break;
        case NC_SHORT:
            bbAppendn(memory,(void*)&target.value.int16v,sizeof(target.value.int16v));
	    break;
        case NC_INT:
            bbAppendn(memory,(void*)&target.value.int32v,sizeof(target.value.int32v));
	    break;
        case NC_FLOAT:
            bbAppendn(memory,(void*)&target.value.floatv,sizeof(target.value.floatv));
	    break;
        case NC_DOUBLE:
            bbAppendn(memory,(void*)&target.value.doublev,sizeof(target.value.doublev));
	    break;
        case NC_UBYTE:
            bbAppendn(memory,(void*)&target.value.uint8v,sizeof(target.value.uint8v));
	    break;
        case NC_USHORT:
            bbAppendn(memory,(void*)&target.value.uint16v,sizeof(target.value.uint16v));
	    break;
        case NC_UINT:
            bbAppendn(memory,(void*)&target.value.uint32v,sizeof(target.value.uint32v));
	    break;
        case NC_INT64: {
	    union SI64 { char ch[8]; long long i64;} si64;
	    si64.i64 = target.value.int64v;
            bbAppendn(memory,(void*)si64.ch,sizeof(si64.ch));
	    } break;
        case NC_UINT64: {
	    union SU64 { char ch[8]; unsigned long long i64;} su64;
	    su64.i64 = target.value.uint64v;
            bbAppendn(memory,(void*)su64.ch,sizeof(su64.ch));
	    } break;
        case NC_STRING: {
            if(usingclassic) {
                bbAppendn(memory,target.value.stringv.stringv,target.value.stringv.len);
            } else if(target.nctype == NC_CHAR) {
                bbAppendn(memory,target.value.stringv.stringv,target.value.stringv.len);
            } else {
                char* ptr;
                int len = (size_t)target.value.stringv.len;
                ptr = poolalloc(len+1); /* CAREFUL: this has short lifetime*/
                memcpy(ptr,target.value.stringv.stringv,len);
                ptr[len] = '\0';
                bbAppendn(memory,(void*)&ptr,sizeof(ptr));
            }
        } break;

        default: PANIC1("gencdf_primdata: unexpected type: %d",target.nctype);
    }
}

static void
gencdf_fillvalue(Symbol* tsym, Symbol* fillsrc, Datasrc* src, Bytebuffer* memory)
{
    Datalist* list = NULL;

    assert(tsym->objectclass == NC_TYPE);
    if(fillsrc != NULL) list = fillsrc->var.special._Fillvalue;
    if(list == NULL) list = getfiller(tsym);
    srcpushlist(src,list);
    gencdf_datalist1(tsym,src,memory,NULL);
    srcpop(src);
}

/*
This walk of the data lists collects
vlen sublists and constructs separate C constants
for each of them. The "id" of each list is then
recorded in the containing datalist.
*/

void
gencdf_vlenconstants(void)
{
    int i,len;
    Datasrc vlensrc;
    Bytebuffer* memory = bbNew();

    len = listlength(vlenconstants);
    if(len == 0) return;
    vlendata = (struct Vlendata*)emalloc(sizeof(struct Vlendata)*len+1);
    memset((void*)vlendata,0,sizeof(struct Vlendata)*len+1);
    /* start at one so that the vlen ids are > 0*/
    for(i=0;i<listlength(vlenconstants);i++) {
	Constant* cmpd = (Constant*)listget(vlenconstants,i);
	Symbol* tsym = cmpd->value.compoundv->schema;
	unsigned long uid = cmpd->value.compoundv->vlen.uid;
	unsigned long count;

        assert(tsym != NULL);
	vlensrc = makedatasrc(cmpd->value.compoundv);
	bbClear(memory);
	count = 0;
	while(srcmore(&vlensrc)) {
            gencdf_datalist1(tsym->typ.basetype,&vlensrc,memory,NULL);
	    count++;
	}
	assert(count == cmpd->value.compoundv->vlen.count);
	vlendata[uid].data = bbDup(memory);
	vlendata[uid].count = count;
    }
    bbFree(memory);
}


static const char zeros[] =
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static void
alignbuffer(Constant* prim, Bytebuffer* buf)
{
    int alignment,pad,offset;

    if(prim->nctype == NC_ECONST)
        alignment = nctypealignment(prim->value.enumv->typ.typecode);
    else if(usingclassic && prim->nctype == NC_STRING)
        alignment = nctypealignment(NC_CHAR);
    else if(prim->nctype == NC_CHAR)
        alignment = nctypealignment(NC_CHAR);
    else
        alignment = nctypealignment(prim->nctype);
    offset = bbLength(buf);
    pad = getpadding(offset,alignment);
    if(pad > 0) {
	bbAppendn(buf,(void*)zeros,pad);
    }
}

static void
gencdf_vardata(Symbol* var, Datalist* data, Bytebuffer* memory, Putvar* closure)
{
    Datasrc src;
    int i,checkpoint = 0;

    assert(data->dimdata != NULL);
    src = makedatasrc(data);
    for(i=0;i<var->typ.dimset.ndims;i++) {
	if(var->typ.dimset.dimsyms[i]->dim.size == NC_UNLIMITED)
	    checkpoint = i; /* indicates when to call put_vara*/
    }
    gencdf_vardata1(var,&src,memory,data->dimdata,closure,0,checkpoint);
}

static void
gencdf_vardata1(Symbol* var,
		Datasrc* src,
		Bytebuffer* memory,
		Odometer* odom,
		Putvar* closure,
		int index,
		int checkpoint)
{
    int i,iscmpd;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int firstdim = (index == 0);
    int useclosure = (closure != NULL);
    int atcheckpoint = (index == checkpoint);
    int declsize = odom->dims[index].declsize;
    int isunlimited = (declsize == 0);
    Symbol* basetype = var->typ.basetype;
    Symbol* fillsrc = var;
    Constant* con;

    assert(index >= 0 && index < rank);

    odom->dims[index].index = 0; /* reset*/

    if(!firstdim ) SRCPUSH(iscmpd,src);

    if(lastdim && basetype->typ.typecode == NC_CHAR) {
        /* If we are trying to get a matrix of characters, then*/
	/* special handling has to be applied in case the characters*/
	/* are embedded in string constants.*/
	Constant str = gen_string((isunlimited?0:declsize),src,NULL);
	bbAppendn(memory,str.value.stringv.stringv,str.value.stringv.len);
	odom->dims[index].index += str.value.stringv.len;
	if(atcheckpoint && useclosure) {
            closure->putvar(closure,odom,memory);
            bbClear(memory);
	}
    } else if(isunlimited) {
	while((srcpeek(src))!=NULL) {
	    if(lastdim)
                gencdf_datalist1(basetype,src,memory,fillsrc);
	    else
                gencdf_vardata1(var,src,memory,odom,closure,index+1,checkpoint);
	    odom->dims[index].index++;
	    if(atcheckpoint && useclosure) {
                closure->putvar(closure,odom,memory);
                bbClear(memory);
	    }
	}
    } else for(i=0;i<declsize;i++) {
	con = srcpeek(src);
	if(con == NULL) break; /* no point in going on*/
	if(lastdim)
            gencdf_datalist1(basetype,src,memory,fillsrc);
	else
            gencdf_vardata1(var,src,memory,odom,closure,index+1,checkpoint);
        odom->dims[index].index++;
	if(atcheckpoint && useclosure) {
            closure->putvar(closure,odom,memory);
        }
    }
    if(!firstdim ) SRCPOP(iscmpd,src);
}



