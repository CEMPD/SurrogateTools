/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/daputil.c,v 1.3 2009/03/25 01:48:28 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"
#include "dapalign.h"

/**************************************************/
/* Provide a hidden interface to allow utilities*/
/* to check if a given path name is really an ncdap3 url.*/
/* If no, return null, else return basename of the url*/
/* minus any extension*/
char*
nc__testurl(const char* path)
{
    DAPURL url;
    char* base = NULL;
    int ok = dapurlparse(path,&url);
    if(ok) {
	char* slash = strrchr(path, '/');
	char* dot;
	if(slash == NULL) slash = (char*)path; else slash++;
        slash = strdup(slash);
	dot = strrchr(slash, '.');
        if(dot != NULL &&  dot != slash) *dot = '\0';
	base = slash;
    }
    dapurlclear(&url);
    return base;
}

/**************************************************/

static char hexchars[16] = {
'0', '1', '2', '3',
'4', '5', '6', '7',
'8', '9', 'a', 'b',
'c', 'd', 'e', 'f',
};

static char cvtchars1[] = "0123456789 !#$%&'()*,:;<=>?[\\]^`{|}~\"\\";
static char cvtcharsn[] = " !#$%&'()*,:;<=>?[\\]^`{|}~\"\\";

/* Given a legal dap name with arbitrary characters,*/
/* convert to equivalent cdf name*/
/* Current algorithm: replace suspect characters*/
/* with 0xHH.*/

char*
cdfbasename(char* dapname)
{
    int c;
    char* newname;
    char* cvtchars;
    NCbytebuffer* buf;
    if(dapname == NULL) return NULL;
    buf = ncbytesnew();
    cvtchars = cvtchars1;
    while((c=*dapname++)) {
	if(c >= 127 || strchr(cvtchars,c) != NULL) {
	    char tmp[8];
	    int hex1, hex2;
	    hex1 = (c & 0x0F);
	    hex2 = (c & 0xF0) >> 4;
	    tmp[0] = '0'; tmp[1] = 'x';
	    tmp[2] = hexchars[hex2] ; tmp[3] = hexchars[hex1];
	    tmp[4] = '\0';	    
	    ncbytescat(buf,tmp);
	} else
	    ncbytesappend(buf,c);
        cvtchars = cvtcharsn; /* for non-first tests*/
    }
    newname = ncbytesdup(buf);
    ncbytesfree(buf);
    return newname;
}

/* Define the type upgrade to hold unsigned types*/
nc_type
nctypeupgrade(nc_type nctype)
{
    switch (nctype) {
    case NC_CHAR:    return NC_CHAR;
    case NC_BYTE:    return NC_BYTE;
    case NC_UBYTE:   return NC_UBYTE;
    case NC_SHORT:   return NC_SHORT;
    case NC_USHORT:  return NC_INT;
    case NC_INT:     return NC_INT;
    case NC_UINT:    return NC_DOUBLE;
    case NC_INT64:   return NC_DOUBLE;
    case NC_UINT64:  return NC_DOUBLE;
    case NC_FLOAT:   return NC_FLOAT;
    case NC_DOUBLE:  return NC_DOUBLE;
    case NC_URL:
    case NC_STRING: return NC_CHAR;
    default: break;
    }
    return NC_NAT;
}

nc_type
octypetonc(OCtype etype)
{
    switch (etype) {
    case OC_Char:	return NC_CHAR;
    case OC_Byte:	return NC_BYTE;
    case OC_UByte:	return NC_UBYTE;
    case OC_Int16:	return NC_SHORT;
    case OC_UInt16:	return NC_USHORT;
    case OC_Int32:	return NC_INT;
    case OC_UInt32:	return NC_UINT;
    case OC_Int64:	return NC_INT64;
    case OC_UInt64:	return NC_UINT64;
    case OC_Float32:	return NC_FLOAT;
    case OC_Float64:	return NC_DOUBLE;
    case OC_String:	return NC_STRING;
    case OC_URL:	return NC_STRING;
    case OC_Dataset:	return NC_Dataset;
    case OC_Sequence:	return NC_Sequence;
    case OC_Structure:	return NC_Structure;
    case OC_Grid:	return NC_Grid;
    case OC_Dimension:	return NC_Dimension;
    case OC_Primitive:	return NC_Primitive;
    default: break;
    }
    return NC_NAT;
}

OCtype
nctypetodap(nc_type nctype)
{
    switch (nctype) {
    case NC_CHAR:	return OC_Char;
    case NC_BYTE:	return OC_Byte;
    case NC_UBYTE:	return OC_UByte;
    case NC_SHORT:	return OC_Int16;
    case NC_USHORT:	return OC_UInt16;
    case NC_INT:	return OC_Int32;
    case NC_UINT:	return OC_UInt32;
    case NC_INT64:	return OC_Int64;
    case NC_UINT64:	return OC_UInt64;
    case NC_FLOAT:	return OC_Float32;
    case NC_DOUBLE:	return OC_Float64;
    case NC_STRING:	return OC_String;
    default : break;
    }
    return OC_NAT;
}

size_t
nctypesizeof(nc_type nctype)
{
    switch (nctype) {
    case NC_CHAR:	return sizeof(char);
    case NC_BYTE:	return sizeof(signed char);
    case NC_UBYTE:	return sizeof(unsigned char);
    case NC_SHORT:	return sizeof(short);
    case NC_USHORT:	return sizeof(unsigned short);
    case NC_INT:	return sizeof(int);
    case NC_UINT:	return sizeof(unsigned int);
    case NC_INT64:	return sizeof(long long);
    case NC_UINT64:	return sizeof(unsigned long long);
    case NC_FLOAT:	return sizeof(float);
    case NC_DOUBLE:	return sizeof(double);
    case NC_STRING:	return sizeof(char*);
    default: PANIC("nctypesizeof");
    }
    return 0;
}

char*
nctypetostring(nc_type nctype)
{
    switch (nctype) {
    case NC_NAT:	return "NC_NAT";
    case NC_BYTE:	return "NC_BYTE";
    case NC_CHAR:	return "NC_CHAR";
    case NC_SHORT:	return "NC_SHORT";
    case NC_INT:	return "NC_INT";
    case NC_FLOAT:	return "NC_FLOAT";
    case NC_DOUBLE:	return "NC_DOUBLE";
    case NC_UBYTE:	return "NC_UBYTE";
    case NC_USHORT:	return "NC_USHORT";
    case NC_UINT:	return "NC_UINT";
    case NC_INT64:	return "NC_INT64";
    case NC_UINT64:	return "NC_UINT64";
    case NC_STRING:	return "NC_STRING";
    case NC_VLEN:	return "NC_VLEN";
    case NC_OPAQUE:	return "NC_OPAQUE";
    case NC_ENUM:	return "NC_ENUM";
    case NC_COMPOUND:	return "NC_COMPOUND";
    case NC_URL:	return "NC_URL";
    case NC_SET:	return "NC_SET";
    default: break;
    }
    return NULL;
}

/* Collect the set of container nodes ending in "container"*/
void
collectnodepath(CDFnode* container, NClist* path)
{
    if(container == NULL) return;
    /* stop at the dataset container as well*/
    if(container->nctype != NC_Dataset)
        collectnodepath(container->container,path);
    if(container->name != NULL) nclistpush(path,(ncelem)container);
}

#ifdef IGNORE
/* Compute the 1+deepest occurrence of a sequence in the path*/
int
dividepoint(NClist* path)
{
    /* find divide point*/
    int i,len = nclistlength(path);
    int divide = 0; /* to indicate not found*/
    for(i=0;i<len;i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	if(node->nctype == NC_Sequence) divide = i+1;
    }
    return divide;
}

/* Divide the set into two parts, those before and including the*/
/* innermost sequence and those below that point*/
void
dividepath(NClist* path, NClist* prefix)
{
    int i,divide;
    divide = dividepoint(path);
    if(divide > 0) { /* move the prefix part if divide >= 0*/
	for(i=0;i<=divide;i++) {
	    ncelem node = nclistget(path,0);
	    nclistpush(prefix,node);
	    nclistremove(path,0);
	}
    }
}
#endif

/* Given a path, collect the set of dimensions along that path*/
/* If in the process, we encounter a sequence, then*/
/* insert an unlimited dimension to cover all dimensions*/
/* thru deepest sequence instance*/
void
collectdims(NCDRNO* drno, NClist* path, NClist* dimset)
{
    int i,j;
    int lastseq = -1;
    for(i=0;i<nclistlength(path);i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	if(node->nctype == NC_Sequence) lastseq = nclistlength(dimset);
	if(node->array.rank == 0) continue;
        for(j=0;j<nclistlength(node->array.dimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(node->array.dimensions,j);
	    nclistpush(dimset,(ncelem)dim);
	}
    }
    /* Now handle the unlimited case*/
    if(lastseq >=  0) {
	while(lastseq-- > 0) nclistremove(dimset,0);
	nclistinsert(dimset,0,(ncelem)drno->cdf.unlimited);
    }    
}

void*
alignbuffer(nc_type nctype, void* buf)
{
    int alignment,pad,offset,rem;
    alignment = ncctypealignment(nctype);
    /* This is a bad thing*/
    offset = ((unsigned long)buf) % alignment;
    rem = (alignment==0?0:(offset % alignment));
    pad = (rem==0?0:(alignment - rem));
    if(pad > 0) memset(buf,0,pad);
    return (void*)(((char*)buf) + pad);
}

