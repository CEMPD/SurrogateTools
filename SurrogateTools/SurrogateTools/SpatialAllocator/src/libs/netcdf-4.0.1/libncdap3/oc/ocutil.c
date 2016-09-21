/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include <unistd.h>
#include <fcntl.h>
#include "ocinternal.h"
#include "ocdebug.h"

/* Null terminate; element 0 is preferred form.*/
static char* DDSdatamarks[] = {"Data:\n", "Data:\r\n",(char*)0};

/* Not all systems have strndup, so provide one*/
char*
ocstrndup(const char* s, size_t len)
{
    char* dup;
    if(s == NULL) return NULL;
    dup = (char*)ocmalloc(len+1);
    MEMCHECK(dup,NULL);
    memcpy((void*)dup,s,len);
    dup[len] = '\0';
    return dup;
}

void
makedimlist(OClist* path, OClist* dims)
{
    unsigned int i,j;
    for(i=0;i<oclistlength(path);i++) {
	OCnode* node = (OCnode*)oclistget(path,i);
        unsigned int rank = node->array.rank;
	for(j=0;j<rank;j++) {
	    OCnode* dim = (OCnode*)oclistget(node->array.dimensions,j);
	    oclistpush(dims,(ocelem)dim);
        }
    }
}

void
ocfreeprojectionclause(Projectionclause* clause)
{
    if(clause->target != NULL) free(clause->target);
    while(oclistlength(clause->indexsets) > 0) {
	OClist* slices = (OClist*)oclistpop(clause->indexsets);
        while(oclistlength(slices) > 0) {
	    OCslice* slice = (OCslice*)oclistpop(slices);
	    if(slice != NULL) free(slice);
	}
        oclistfree(slices);
    }
    oclistfree(clause->indexsets);
    free(clause);
}

static void
freeAttributes(OClist* attset)
{
    unsigned int i,j;
    for(i=0;i<oclistlength(attset);i++) {
	Attribute* att = (Attribute*)oclistget(attset,i);
	if(att->name != NULL) free(att->name);
	if(att->etype == OC_String || att->etype == OC_URL) {
	    for(j=0;j<att->nvalues;j++) {
		char* s = ((char**)att->values)[j];
		if(s != NULL) free(s);
	    }
	} else {
	    free(att->values);
	}
    }
}

void
freeOCnode(OCnode* cdf, int deep)
{
    unsigned int i;
    if(cdf == NULL) return;
    if(cdf->name != NULL) free(cdf->name);
    if(cdf->fullname != NULL) free(cdf->fullname);
    if(cdf->attributes != NULL) freeAttributes(cdf->attributes);
    if(cdf->subnodes != NULL) {
	if(deep) {
            for(i=0;i<oclistlength(cdf->subnodes);i++) {
	        OCnode* node = (OCnode*)oclistget(cdf->subnodes,i);
		freeOCnode(node,deep);
	    }
	}
        oclistfree(cdf->subnodes);
    }
    free(cdf);
}

int
findbod(OCbytes* buffer, size_t* bodp, size_t* ddslenp)
{
    unsigned int i;
    char* content;
    size_t len = ocblength(buffer);

    content = ocbcontents(buffer);
    for(i=0;i<len;i++) {
	char** tagp;
	for(tagp=DDSdatamarks;*tagp;tagp++) {
	    int tlen = strlen(*tagp);
	    if((i+tlen) <= len && strncmp(content+i,*tagp,tlen)==0) {
		*ddslenp = i;
	        *bodp = (i+tlen);
	        return 1;
	    }
	}
    }
    return 0; /* tag not found; not necessarily an error*/
}

/* Compute total # of elements if dimensioned*/
size_t
totaldimsize(OCnode* node)
{
    unsigned int i;
    size_t count = 1;
    for(i=0;i<node->array.rank;i++) {
        OCnode* dim = (OCnode*)oclistget(node->array.dimensions,i);
        count *= (dim->dim.declsize);
    }
    return count;
}

#ifdef IGNORE
size_t
totaldimsize(unsigned int rank, size_t* dimsizes)
{
    unsigned int i;
    int unlim = 0;
    unsigned long size = 1;
    for(i=0;i<rank;i++) {
        if(dimsizes[i] != 0) size = (size * dimsizes[i]); else unlim = 1;
    }
    return size;
}
#endif

size_t
octypesize(OCtype etype)
{
    switch (etype) {
    case OC_Char:	return sizeof(char);
    case OC_Byte:	return sizeof(signed char);
    case OC_UByte:	return sizeof(unsigned char);
    case OC_Int16:	return sizeof(short);
    case OC_UInt16:	return sizeof(unsigned short);
    case OC_Int32:	return sizeof(int);
    case OC_UInt32:	return sizeof(unsigned int);
    case OC_Int64:	return sizeof(long long);
    case OC_UInt64:	return sizeof(unsigned long long);
    case OC_Float32:	return sizeof(float);
    case OC_Float64:	return sizeof(double);
    case OC_String:	return sizeof(char*);
    case OC_URL:	return sizeof(char*);
		  /* Ignore all others */
    default: break;
    }
    return 0;
}

char*
octypetostring(OCtype octype)
{
    switch (octype) {
    case OC_NAT:          return "OC_NAT";
    case OC_Char:         return "OC_Char";
    case OC_Byte:         return "OC_Byte";
    case OC_UByte:	   return "OC_UByte";
    case OC_Int16:        return "OC_Int16";
    case OC_UInt16:       return "OC_UInt16";
    case OC_Int32:        return "OC_Int32";
    case OC_UInt32:       return "OC_UInt32";
    case OC_Int64:        return "OC_Int64";
    case OC_UInt64:       return "OC_UInt64";
    case OC_Float32:      return "OC_Float32";
    case OC_Float64:      return "OC_Float64";
    case OC_String:       return "OC_String";
    case OC_URL:          return "OC_URL";
    /* Non-primitives*/
    case OC_Dataset:      return "OC_Dataset";
    case OC_Sequence:     return "OC_Sequence";
    case OC_Grid:         return "OC_Grid";
    case OC_Structure:    return "OC_Structure";
    case OC_Dimension:    return "OC_Dimension";
    case OC_Attribute:    return "OC_Attribute";
    case OC_Attributeset: return "OC_Attributeset";
    case OC_Primitive:    return "OC_Primitive";
    default: break;
    }
    return NULL;
}


char*
octypeprint(OCtype etype, char* buf, size_t bufsize, void* value)
{
    if(buf == NULL || bufsize == 0 || value == NULL) return NULL;
    buf[0] = '\0';
    switch (etype) {
    case OC_Char:
	snprintf(buf,bufsize,"'%c'",*(char*)value);
	break;
    case OC_Byte:
	snprintf(buf,bufsize,"%d",*(signed char*)value);
	break;
    case OC_UByte:
	snprintf(buf,bufsize,"%u",*(unsigned char*)value);
	break;
    case OC_Int16:
	snprintf(buf,bufsize,"%d",*(short*)value);
	break;
    case OC_UInt16:
	snprintf(buf,bufsize,"%u",*(unsigned short*)value);
	break;
    case OC_Int32:
	snprintf(buf,bufsize,"%d",*(int*)value);
	break;
    case OC_UInt32:
	snprintf(buf,bufsize,"%u",*(unsigned int*)value);
	break;
    case OC_Int64:
	snprintf(buf,bufsize,"%lld",*(long long*)value);
	break;
    case OC_UInt64:
	snprintf(buf,bufsize,"%llu",*(unsigned long long*)value);
	break;
    case OC_Float32:
	snprintf(buf,bufsize,"%g",*(float*)value);
	break;
    case OC_Float64:
	snprintf(buf,bufsize,"%g",*(double*)value);
	break;
    case OC_String:
    case OC_URL: {
	char* s = *(char**)value;
	snprintf(buf,bufsize,"\"%s\"",s);
	} break;
    default: break;
    }
    return buf;
}

size_t
ocxdrsize(OCtype etype)
{
    switch (etype) {
    case OC_Char:
    case OC_Byte:
    case OC_UByte:
    case OC_Int16:
    case OC_UInt16:
    case OC_Int32:
    case OC_UInt32:
	return BYTES_PER_XDR_UNIT;
    case OC_Int64:
    case OC_UInt64:
	return (2*BYTES_PER_XDR_UNIT);
    case OC_Float32:
	return BYTES_PER_XDR_UNIT;
    case OC_Float64:
	return (2*BYTES_PER_XDR_UNIT);
    case OC_String:
    case OC_URL:
    default: break;
    }
    return 0;
}

/***********************************/
/* Skip "len" bytes in the input*/
int
xdr_skip(XDR* xdrs, unsigned int len)
{
    unsigned int pos;
    if(len <= 0) return 1; /* ignore*/
    pos = xdr_getpos(xdrs);
    return xdr_setpos(xdrs,(pos+RNDUP(len)));
}

/* skip "n" string/bytestring instances in the input*/
int
xdr_skip_strings(XDR* xdrs, unsigned int n)
{
    while(n-- > 0) {
        unsigned int slen;
	if(!xdr_u_int(xdrs,&slen)) return xdrerror();
	if(xdr_skip(xdrs,RNDUP(slen))) return xdrerror();
    }
    return THROW(OC_NOERR);
}

unsigned int xdr_roundup(unsigned int n)
{
    unsigned int rem;
    rem = (n % BYTES_PER_XDR_UNIT);
    if(rem > 0) n += (BYTES_PER_XDR_UNIT - rem);
    return n;
}

/**************************************/

char*
ocerrstring(int err)
{
    if(err == 0) return "no error";
    if(err > 0) return strerror(err);
    switch (err) {
    case OC_NOERR: return "OC_NOERR";
    case OC_EBADID: return "OC_EBADID";
    case OC_ECHAR: return "OC_ECHAR";
    case OC_EDIMSIZE: return "OC_EDIMSIZE";
    case OC_EEDGE: return "OC_EEDGE";
    case OC_EINVAL: return "OC_EINVAL";
    case OC_EINVALCOORDS: return "OC_EINVALCOORDS";
    case OC_ENOMEM: return "OC_ENOMEM";
    case OC_ENOTVAR: return "OC_ENOTVAR";
    case OC_EPERM: return "OC_EPERM";
    case OC_ESTRIDE: return "OC_ESTRIDE";
    case OC_EDAP: return "OC_EDAP";
    case OC_EXDR: return "OC_EXDR";
    case OC_ECURL: return "OC_ECURL";
    case OC_EBADURL: return "OC_EBADURL";
    case OC_EBADVAR: return "OC_EBADVAR";
    case OC_EOPEN: return "OC_EOPEN";
    case OC_EIO: return "OC_EIO";
    default: break;
    }
    return "<unknown error>";
}
