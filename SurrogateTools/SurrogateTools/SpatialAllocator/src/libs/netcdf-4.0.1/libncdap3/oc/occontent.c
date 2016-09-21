/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include "ocdata.h"
#include "occontent.h"
#include "ocdebug.h"

/* Mnemonic*/
#define ISPACKED 1

/* Forward*/
static OCmode modetransition(OCnode* node, OCmode mode);
static int ocgetmemdata(OCconnection*, OCcontent*, void* memory, size_t memsize,
                        size_t start, size_t count);

OCcontent*
oc_newcontent(OCconnection* state)
{
    OCcontent* content;
    if(state == NULL) return NULL;
    content = state->content.contentlist;
    /* Search for an unused content node*/
    while(content != NULL && content->mode != Emptymode) {content = content->next;}
    if(content == NULL) {
	content = (OCcontent*)ocmalloc(sizeof(OCcontent));
	MEMCHECK(content,(OCcontent*)NULL);
        content->next = state->content.contentlist;
        state->content.contentlist = content;
    }
    return oc_resetcontent(state,content);
}

void
oc_freecontent(OCconnection* state, OCcontent* content)
{
    if(content != NULL) {content->mode = Emptymode;}
}

OCcontent*
oc_resetcontent(struct OCconnection* state, OCcontent* content)
{
    content->state = state;
    content->mode = Nullmode;
    content->node = NULL;
    content->xdroffset = 0;
    content->index = 0;
    content->packed = 0;
    content->memdata = NULL;
    return content;
}

/* Copy everything except public and private fields*/
OCcontent*
oc_clonecontent(OCconnection* state, OCcontent* content)
{
    OCcontent* clone = oc_newcontent(state);
    clone->mode = content->mode;
    clone->node = content->node;
    clone->xdroffset = content->xdroffset;
    clone->index = content->index;
    clone->memdata = content->memdata;
    return clone;
}

int
oc_rootcontent(OCconnection* state, OCcontent* content)
{
    if(state == NULL || content == NULL) return THROW(OC_EINVAL);
    if(state->schema.datadds.nodes == NULL) return THROW(OC_EINVAL);
    if(state->content.memdata == NULL  && state->dap.xdrs == NULL) return THROW(OC_EXDR);
    oc_resetcontent(state,content);
    content->state = state; /* make sure*/
    content->mode = Fieldmode;
    content->node = state->schema.datadds.tree;
    if(state->content.memdata == NULL)
        content->xdroffset = state->dap.bod;
    else {
	content->memdata = state->content.memdata;
        content->mode = content->memdata->mode;
    }
    return THROW(OC_NOERR);
}

int
oc_dimcontent(OCconnection* state, OCcontent* content, OCcontent* newcontent, size_t index)
{
    unsigned int i;
    int stat = OC_NOERR;
    XDR* xdrs;
    unsigned int xdrcount;
    int packed;
    OCtype etype,octype;

    if(state == NULL || content == NULL) return THROW(OC_EINVAL);
    if(content->mode != Dimmode) return THROW(OC_EINVAL);
    if(content->node->array.rank == 0) return THROW(OC_EINVAL);

    etype = content->node->etype;
    octype = content->node->octype;

    content->index = index; /* Track our location in parent content */

    /* check if the data is packed*/
    packed = (octype == OC_Primitive &&
              (etype == OC_Byte || etype == OC_UByte || etype == OC_Char));

    oc_resetcontent(state,newcontent);
    /* Set the some of the new content*/
    newcontent->state = state; /* make sure*/
    newcontent->node = content->node; /* keep same node*/
    newcontent->packed = packed;
    newcontent->mode = modetransition(newcontent->node, content->mode);
    newcontent->index = 0;

    if(content->memdata != NULL) { /* Get data from the compiled version */
        OCASSERT((content->memdata->mode == Dimmode));
	OCASSERT((content->memdata->count > index));
        /* Leave the primitive alone to be picked up by oc_getcontent */
	if(octype != OC_Primitive) {
   	    OCmemdata* next;
	    OCASSERT((octype == OC_Structure));
   	    next = ((OCmemdata**)content->memdata->data.data)[index];
	    newcontent->memdata = next;
	} else
	    newcontent->memdata = content->memdata; /* use same memdata */
	return OC_NOERR;
    }

    xdrs = state->dap.xdrs;
    if(xdrs == NULL) return THROW(OC_EXDR);

    /* checkpoint the beginning of this instance*/
    if(content->xdroffset == 0) content->xdroffset = xdr_getpos(xdrs);
    /* move to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    /* Collect the dimension count from the xdr data packet*/
    if(!xdr_u_int(xdrs,&xdrcount)) return xdrerror();
    if(xdrcount < index) return THROW(OC_EINVALCOORDS);

    /* pull out redundant second count*/
    /* (note that String/URL do not redundant count)*/
    if(octype == OC_Primitive && etype != OC_String && etype != OC_URL) {
        if(!xdr_u_int(xdrs,&xdrcount)) return xdrerror();
    }

    /* We have to treat OC_Byte/UByte/Char specially*/
    /* because the data is packed in the xdr packet*/
    if(packed) {
	/* In effect, compile where the data is, but wait*/
	/* until a getcontent to retrieve it*/
	OCASSERT((newcontent->mode == Datamode));
        newcontent->index = index; /* record the final destination in the packed data*/
	newcontent->packed = 1;
	return THROW(OC_NOERR);
    }

    for(i=0;i<index;i++) {
        stat = oc_skipinstance(content->node,xdrs);
        if(stat != OC_NOERR) return THROW(stat);
    }
    /* Record the location of the newcontent */
    newcontent->xdroffset = xdr_getpos(xdrs);

    /* move back to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    return THROW(OC_NOERR);
}

int
oc_recordcontent(OCconnection* state,OCcontent* content, OCcontent* recordcontent, size_t index)
{
    unsigned int i;
    int stat = OC_NOERR;
    XDR* xdrs;
    char tmp[BYTES_PER_XDR_UNIT];
    OCtype octype,etype;
    int packed;

    if(state == NULL || content == NULL) return THROW(OC_EINVAL);
    if(content->mode != Recordmode) return THROW(OC_EINVAL);

    content->index = index; /* Track our location in parent content */

    octype = content->node->octype;
    etype = content->node->etype;

    /* check if the data is packed*/
    packed = (octype == OC_Primitive &&
              (etype == OC_Byte || etype == OC_UByte || etype == OC_Char));

    oc_resetcontent(state,recordcontent);
    /* Set some of the new content*/
    recordcontent->state = state; /* make sure*/
    recordcontent->node = content->node;
    recordcontent->packed = packed;
    recordcontent->mode = modetransition(recordcontent->node, content->mode);
    recordcontent->index = 0;

    if(content->memdata != NULL) { /* Get data from the compiled version */
	OCmemdata* next;
        OCASSERT((content->memdata->mode == Recordmode));
	if(content->memdata->count <= index) return THROW(OC_EINVALCOORDS);
	next = ((OCmemdata**)content->memdata->data.data)[index];
	recordcontent->memdata = next;
	return OC_NOERR;
    }

    xdrs = state->dap.xdrs;
    if(xdrs == NULL) return THROW(OC_EXDR);

    /* checkpoint the beginning of this instance*/
    if(content->xdroffset == 0) content->xdroffset = xdr_getpos(xdrs);

    /* move to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    switch (content->node->octype) {
    case OC_Sequence:
        for(i=0;i<index;i++) {
            /* pick up the sequence record begin marker*/
            /* extract the tag byte*/
            if(!xdr_opaque(xdrs,tmp,sizeof(tmp))) return xdrerror();
            if(tmp[0] == StartOfoclist) {
                /* skip instance*/
                stat = oc_skipinstance(content->node,xdrs);
            } else if(tmp[0] == EndOfoclist) {
                stat = OC_EINVALCOORDS; /* not enough records*/
                break;
            } else {
                oclog(LOGERR,"missing/invalid begin/end record marker\n");
                stat = OC_EINVAL;
                break;
            }
        }
	if(stat != OC_NOERR) return THROW(stat);

	/* skip the sequence begin marker for the chosen record*/
	/* so we are at its contents*/
        if(!xdr_opaque(xdrs,tmp,sizeof(tmp))) return xdrerror();
        if(tmp[0] != StartOfoclist) return THROW(OC_EINVALCOORDS);

	/* Set contents of the output content*/
        recordcontent->xdroffset = xdr_getpos(xdrs);
	break;

    case OC_Dataset:
    case OC_Structure:
    case OC_Grid:
    case OC_Primitive:
    default: return THROW(OC_EINVAL);
    }

    /* move back to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    return THROW(OC_NOERR);
}

int
oc_fieldcontent(OCconnection* state, OCcontent* content, OCcontent* fieldcontent, size_t index)
{
    unsigned int i;
    int stat = OC_NOERR;
    XDR* xdrs;
    OCtype octype,etype;
    int packed;

    if(state == NULL || content == NULL) return THROW(OC_EINVAL);
    if(content->mode != Fieldmode) return THROW(OC_EINVAL);

    content->index = index; /* Track our location in parent content */

    octype = content->node->octype;
    etype = content->node->etype;

    /* check if the data is packed*/
    packed = (octype == OC_Primitive &&
              (etype == OC_Byte || etype == OC_UByte || etype == OC_Char));

    oc_resetcontent(state,fieldcontent);
    /* Set the some of the new content*/
    fieldcontent->state = state; /* make sure*/
    fieldcontent->node = (OCnode*)oclistget(content->node->subnodes,index);
    fieldcontent->packed = packed;
    fieldcontent->mode = modetransition(fieldcontent->node, content->mode);
    fieldcontent->index = 0; /* record where we want to be */

    if(content->memdata != NULL) { /* Get data from the compiled version */
	OCmemdata* md = content->memdata;
	OCmemdata* next;
        OCASSERT((md->mode == Fieldmode));
	OCASSERT((md->count > index));
	next = ((OCmemdata**)md->data.data)[index];
	fieldcontent->memdata = next;
	return OC_NOERR;
    }

    xdrs = state->dap.xdrs;
    if(xdrs == NULL) return THROW(OC_EXDR);

    /* checkpoint the beginning of this instance*/
    if(content->xdroffset == 0) content->xdroffset = xdr_getpos(xdrs);

    /* move to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    switch (content->node->octype) {
    case OC_Grid:
	/* Note that the Grid array is field 0 and the maps are 1..nsubnodes*/
    case OC_Sequence:
    case OC_Dataset:
    case OC_Structure:
	if(index >= oclistlength(content->node->subnodes)) return THROW(OC_EINVALCOORDS);
        for(i=0;i<index;i++) {
  	    OCnode* field = (OCnode*)oclistget(content->node->subnodes,i);
	    stat = oc_skip(field,xdrs);
	    if(stat != OC_NOERR) return THROW(stat);
        }
        fieldcontent->xdroffset = xdr_getpos(xdrs);
	break;

    case OC_Primitive:
    default: return THROW(OC_EINVAL);
    }

    /* move back to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

    return THROW(OC_NOERR);
}

/*
In order to actually extract data,
one must move to the specific primitive typed
field containing the data of interest by using
oc_fieldcontent().
Then, oc_getcontent() is invoked to extract
some subsequence of items from the field.
Note that oc_getcontent() will also work for scalars,
but the start must zero and the count must be one.
*/

int
oc_getcontent(OCconnection* state, OCcontent* content, void* memory, size_t memsize,
                 size_t start, size_t count)
{
    int stat = OC_NOERR;
    XDR* xdrs;
    OCtype etype;
    int isscalar;
    size_t elemsize, totalsize;

    if(state == NULL || content == NULL || memory == NULL) return THROW(OC_EINVAL);
    if(content->node->octype != OC_Primitive) return THROW(OC_EINVAL);

    etype = content->node->etype;
    isscalar = (content->node->array.rank == 0);
    if(isscalar && (start != 0 || count != 1)) return THROW(OC_EINVALCOORDS);

    /* validate memory space*/
    elemsize = octypesize(etype);
    totalsize = elemsize*count;
    if(memsize < totalsize) return THROW(OC_EINVAL);

    OCASSERT((oc_contentmode(state,content)==Dimmode || isscalar));

    if(content->memdata != NULL) { /* Get data from the compiled version */
	stat = ocgetmemdata(state,content,memory,memsize,start,count);
    } else {
        int packed;
	unsigned int xdrcount;
        /* No memdata => use xdr */
        xdrs = state->dap.xdrs;
        if(xdrs == NULL) return THROW(OC_EXDR);

        /* check if the data is packed*/
        packed = (!isscalar && (etype == OC_Byte || etype == OC_UByte || etype == OC_Char));

	content->packed = packed;

        /* Make sure we are at the proper offset: ie at count if !scalar */
        if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();

	if(!isscalar) {
            /* Collect the dimension count from the xdr data packet*/
            if(!xdr_u_int(xdrs,&xdrcount)) return xdrerror();
            if(xdrcount < start) return THROW(OC_EINVALCOORDS);
            if(xdrcount < start+count) return THROW(OC_EINVALCOORDS);
            /* pull out redundant second count*/
            /* (note that String/URL do not have redundant count)*/
            if(etype != OC_String && etype != OC_URL) {
                if(!xdr_u_int(xdrs,&xdrcount)) return xdrerror();
            }
	}
	/* Extract the data */
        stat = oc_xdrread(xdrs,(char*)memory,memsize,packed,content->node->etype,start,count);
        if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror(); /* restore location*/
    }
    return THROW(stat);
}

static int
ocgetmemdata(OCconnection* state, OCcontent* content, void* memory, size_t memsize,
             size_t start, size_t count)
{
    OCmemdata* md = content->memdata;
    unsigned short* d16;
    unsigned int* d32;
    unsigned long long* d64;
    char* dchar;
    char** dstring;
    size_t totalsize;
    size_t elemsize;
    OCtype etype;

    /* Validate the etypes */
    etype = content->node->etype;
    if(etype != md->etype) return THROW(OC_EINVAL);

    elemsize = octypesize(etype);
    totalsize = elemsize*count;

    switch (etype) {
    case OC_Char: case OC_Byte: case OC_UByte:
	dchar = (char*)md->data.data;
	memcpy((void*)memory,(void*)(dchar+start),totalsize);
	break;
    case OC_Int16: case OC_UInt16:
	d16 = (unsigned short*)md->data.data;
	memcpy((void*)memory,(void*)(d16+start),totalsize);	    
	break;
    case OC_Int32: case OC_UInt32: case OC_Float32:
	d32 = (unsigned int*)md->data.data;
	memcpy((void*)memory,(void*)(d32+start),totalsize);
	break;
    case OC_Int64: case OC_UInt64: case OC_Float64:
	d64 = (unsigned long long*)md->data.data;
	memcpy((void*)memory,(void*)(d64+start),totalsize);	    
	break;
    case OC_String: case OC_URL: {
	unsigned int i;
	char** memstrings = (char**)memory;
	dstring = (char**)md->data.data;
	for(i=0;i<count;i++) {
	    memstrings[i] = strdup(dstring[start+i]);
	}
	} break;
    default: OCPANIC1("unexpected etype: %d",content->node->etype);
    }
    return THROW(OC_NOERR);
}

size_t
oc_fieldcount(OCconnection* state, OCcontent* content)
{
    OCnode* node = content->node;
    size_t count;
    OCASSERT((node != NULL));
    count = oclistlength(node->subnodes);
    /* If we are using memdata; then verify against the memdata */
    if(content->memdata != NULL) {
	OCASSERT(content->memdata->count == count);
    }
    return count;
}

/* Extract the actual element count from the xdr packet*/
size_t
oc_dimcount(OCconnection* state, OCcontent* content)
{
    unsigned int count;
    unsigned int xdrsave;
    OCnode* node = content->node;
    XDR* xdrs;

    OCASSERT((node != NULL));
    OCASSERT((content->mode == Dimmode));

    count = totaldimsize(node);

    /* If we are using memdata; then use that to verify */
    if(content->memdata != NULL) {
	OCASSERT(content->memdata->count == count);
	return (size_t)count;
    }

    /* Otherwise verify against xdr */
    xdrs = state->dap.xdrs;
    OCASSERT((xdrs != NULL));

    /* checkpoint current location */
    xdrsave = xdr_getpos(xdrs);

    /* move to content location*/
    OCASSERT(content->xdroffset != 0);
    if(!xdr_setpos(xdrs,content->xdroffset)) return 0;

    /* extract the count*/
    if(!xdr_u_int(xdrs,&count)) count = 0;

    /* return to checkpoint position*/
    if(!xdr_setpos(xdrs,xdrsave)) return 0;

    return (size_t)count;
}

/* Counting records actually requires walking the xdr packet
   so it is not necessarily cheap*/
size_t
oc_recordcount(OCconnection* state, OCcontent* content)
{
    int stat = OC_NOERR;
    size_t count;
    OCnode* node = content->node;
    XDR* xdrs;
    char tmp[BYTES_PER_XDR_UNIT];

    OCASSERT((node != NULL));
    OCASSERT((node->octype == OC_Sequence));
    OCASSERT((content->mode == Recordmode));

    /* If we are using memdata; then use that value */
    if(content->memdata != NULL) {
	return content->memdata->count;
    }

    xdrs = state->dap.xdrs;
    OCASSERT((xdrs != NULL));

    /* checkpoint the beginning of this instance*/
    if(content->xdroffset == 0) content->xdroffset = xdr_getpos(xdrs);

    /* move to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return 0;

    for(count=0;;count++) {
        /* pick up the sequence record begin marker*/
        /* extract the tag byte*/
        if(!xdr_opaque(xdrs,tmp,sizeof(tmp))) return 0;
        if(tmp[0] == StartOfoclist) {
            /* skip instance*/
	    stat = oc_skipinstance(content->node,xdrs);
            if(stat != OC_NOERR) return 0;
        } else if(tmp[0] == EndOfoclist) {
            break; /* done with the count*/
        } else {
            oclog(LOGERR,"missing/invalid begin/end record marker\n");
	    return 0;
	}
    }

    /* move back to checkpoint position*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return 0;

    return count;
}

static OCmode
modetransition(OCnode* node, OCmode srcmode)
{
    switch (srcmode) {
    case Dimmode:
	if(node->octype == OC_Sequence) return Recordmode;
	if(node->octype == OC_Primitive) return Datamode;
	return Fieldmode;

    case Recordmode:
	if(node->array.rank > 0) return Dimmode;
	if(node->octype == OC_Primitive) return Datamode;
	return Fieldmode;

    case Datamode:
	return Datamode;

    case Nullmode:
    case Fieldmode:
	if(node->array.rank > 0) return Dimmode;
	if(node->octype == OC_Primitive) return Datamode;
	if(node->octype == OC_Sequence) return Recordmode;
	return Fieldmode;

    case Emptymode:
    default:
        OCPANIC1("No defined mode transition: %d",(int)srcmode);
	break;
    }
    return Fieldmode;
}

OCmode
oc_contentmode(OCconnection* conn, OCcontent* content) { return ((content)->mode);}

OCnode*
oc_contentnode(OCconnection* conn, OCcontent* content) { return ((content)->node);}

size_t
oc_contentindex(OCconnection* conn, OCcontent* content) { return ((content)->index);}

