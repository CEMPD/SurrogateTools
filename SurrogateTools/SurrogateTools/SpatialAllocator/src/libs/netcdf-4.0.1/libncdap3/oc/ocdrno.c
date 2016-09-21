/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include "ocdata.h"
#include "occontent.h"
#include "ocdebug.h"

/*
This file exports procedures
that access the internals of
oc. They are intended to be called
by the drno code to avoid at least
the appearance of breaking the oc
encapsulation.
*/

/* DRNO need to explicitly get and walk string values*/
int
oc_stringcontent(OCconnection* state, OCcontent* content, char** stringp, size_t* slenp)
{
    int stat = OC_NOERR;
    XDR* xdrs;
    unsigned int slen;
    char* stringmemory;

    if(state == NULL || content == NULL) return THROW(OC_EINVAL);

    if(content->node->octype != OC_Primitive) return THROW(OC_EINVAL);
    if(content->node->etype != OC_String
       && content->node->etype != OC_URL) return THROW(OC_EINVAL);

    xdrs = state->dap.xdrs;
    if(xdrs == NULL) return THROW(OC_EXDR);

    if(oc_contentmode(state,content) != Datamode) return THROW(OC_EINVAL);
    /* We are at a single instance of a string data type*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();
    if(!xdr_u_int(xdrs,&slen)) return xdrerror();
    stringmemory = (char*)ocmalloc(slen+1);
    MEMCHECK(stringmemory,OC_ENOMEM);
    if(!xdr_opaque(xdrs,stringmemory,slen)) return xdrerror();
    stringmemory[slen] = '\0';
    /* restore location*/
    if(!xdr_setpos(xdrs,content->xdroffset)) return xdrerror();
    if(stringp != NULL) *stringp = stringmemory;
    if(slenp != NULL) *slenp = slen;
    return THROW(stat);
}





#ifdef IGNORE
void
oc_addattribute(OCconnection* ocstate, Attribute* attr, OCnode* parent)
{
    addattribute(attr,parent);
}

Attribute*
oc_makeattribute(OCconnection* ocstate, char* name, OCtype ptype, OClist* values)
{
    Attribute* node = makeattribute(name,ptype,values);
    return node;
}
#endif

