/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdump.h,v 1.3 2009/03/25 01:48:28 dmh Exp $
 *********************************************************************/
#ifndef DUMP_H
#define DUMP_H

typedef struct Dimschema {
    int dimid;
/*    int cloneid;*/
    size_t size;
    char name[NC_MAX_NAME+1];
} Dim;

typedef struct Varschema {
    int varid;
/*    int cloneid;*/
    char name[NC_MAX_NAME+1];
    nc_type nctype;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t nelems; /*# elements*/
    size_t alloc; /* malloc size*/
    int natts;
    Attribute* atts;
} Var;

typedef struct NChdr {
    int ncid;
    int format;
    int ndims;
    int nvars;
    int ngatts;
    int unlimid; /* id of the (1) unlimited dimension*/
    Dim* dims;
    Var* vars;
    Attribute* gatts;
    NCbytebuffer* content;
} NChdr;

extern int dumpmetadata(int ncid, NChdr**);
extern void dumpdata1(nc_type nctype, size_t index, char* data);

#endif /*DUMP_H*/
