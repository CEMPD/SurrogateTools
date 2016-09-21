/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/getvar3.h,v 1.1 2009/03/25 01:48:29 dmh Exp $
 *********************************************************************/
#ifndef GETVAR_H
#define GETVAR_H

/*
Store the relevant parameters for accessing
data for a particular variable
Break up the startp, countp, stridep into slices
to facilitate the odometer walk
*/

typedef struct Getvar {
    int nslices;
    NCslice slices[OC_MAX_DIMS];
    void* memory; /* where result is put*/
    /* associated variable*/
    OCtype dsttype;
    CDFnode* target;
} Getvar;

/* Define a tracker for memory to support*/
/* the concatenation*/

struct NCMEMORY {
    void* memory;
    char* next; /* where to store the next chunk of data*/
}; 

#endif /*GETVAR_H*/
