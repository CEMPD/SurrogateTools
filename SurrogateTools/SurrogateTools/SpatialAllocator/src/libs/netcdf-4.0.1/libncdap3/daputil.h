/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/daputil.h,v 1.3 2009/03/25 01:48:29 dmh Exp $
 *********************************************************************/
#ifndef UTIL_H
#define UTIL_H 1

/* why?*/
struct CDFnode;

extern int e(int);

extern OCtype octypeupgrade(OCtype etype);
extern nc_type octypetonc(OCtype etype);
extern OCtype nctypetodap(nc_type ncype);
extern size_t nctypesizeof(nc_type ncype);
extern char* nctypetostring(nc_type nctype);
extern char* maketmppath(char* path, char* prefix);

extern void collectnodepath(struct CDFnode*, NClist* path);

extern void collectdims(struct NCDRNO*, NClist* path, NClist* dimset);

extern char* cdfbasename(char* dapname);

extern void* alignbuffer(nc_type nctype1, void* buf);

#endif /*UTIL_H*/
