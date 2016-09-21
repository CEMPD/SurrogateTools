/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapodom.h,v 1.1 2009/03/25 01:48:28 dmh Exp $
 *********************************************************************/
/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef ODOM_H
#define ODOM_H 1

typedef struct NCslice {
    size_t first;
    size_t count;
    size_t stride;
    size_t stop; /* == first + count*/
    size_t declsize;  /* from defining dimension, if any.*/
} NCslice;

typedef struct Dapodometer {
    int            rank;
    NCslice        slices[OC_MAX_DIMS];
    size_t         index[OC_MAX_DIMS];
} Dapodometer;

/* Odometer operators*/
extern Dapodometer* newdapodometer(NCslice* slices, int first, int count);
extern void freedapodometer(Dapodometer*);
extern char* dapodometerprint(Dapodometer* odom);

extern int dapodometermore(Dapodometer* odom);
extern int dapodometerincr(Dapodometer* odo);
extern int dapodometerincrith(Dapodometer* odo,int);
extern size_t dapodometercount(Dapodometer* odo);
extern void dapodometerreset(Dapodometer*);
extern size_t dapodometertotal(Dapodometer*);
extern Dapodometer* dapodometersplit(Dapodometer* odom, int tail);

#endif /*ODOM_H*/
