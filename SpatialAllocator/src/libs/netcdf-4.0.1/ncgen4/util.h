#ifndef NCGEN_UTIL_H
#define NCGEN_UTIL_H
/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/util.h,v 1.5 2009/03/11 18:26:19 dmh Exp $
 *********************************************************************/

extern void expe2d(char*);
extern int pow2(int);
extern void tztrim(char*);
extern unsigned int chartohex(char c);

extern void reclaimvardata(List*);
extern void reclaimattptrs(void*, long);
extern void cleanup(void);
extern char* fullname(Symbol*);

extern int isunlimited0(Dimset*);
extern int hasunlimited(Dimset* dimset);
extern int classicunlimited(Dimset* dimset);
extern int isbounded(Dimset* dimset);
extern char* nctypename(nc_type);
extern char* ncclassname(nc_class);
extern int ncsize(nc_type);

/* We have several versions of primitive testing*/
extern int isclassicprim(nc_type); /* a classic primitive type*/
extern int isclassicprimplus(nc_type); /* classic + String*/
extern int isprim(nc_type); /* a netcdf4 primitive type*/
extern int isprimplus(nc_type); /* a netcdf4 primitive type + OPAQUE + ENUM*/

extern void collectpath(Symbol* grp, List* grpstack);
extern List* prefixdup(List*);
extern int prefixeq(List*,List*);
#define prefixlen(sequence) (listlength(sequence))

extern char* poolalloc(size_t);
extern char* pooldup(char*);
extern char* poolcat(const char* s1, const char* s2);

extern unsigned long arraylength(Dimset* dimset);
extern unsigned long subarraylength(Dimset* dimset, int first, int last);
extern unsigned char* makebytestring(char* s, size_t* lenp);

extern int getpadding(int offset, int alignment);

extern Datalist* builddatalist(int initialize);
extern void dlappend(Datalist*, Constant*);
extern Constant builddatasublist(Datalist* dl);
extern void dlextend(Datalist* dl);

#endif /*NCGEN_UTIL_H*/
