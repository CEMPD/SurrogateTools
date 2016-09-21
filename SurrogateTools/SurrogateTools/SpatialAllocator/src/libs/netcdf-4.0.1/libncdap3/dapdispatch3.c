/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdispatch3.c,v 1.7 2009/03/25 01:48:28 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"
#include "daprename.h"
#include "dapdebug.h"
#include "dispatch3.h"

extern int
nc3d_getvarx(int ncid, int varid,
            const size_t *startp,
            const size_t *countp,
            const ptrdiff_t* stridep,
            void *value,
            nc_type dsttype);

/* Forward*/
static int nc3d_getvar1(int ncid, int varid,
                       const size_t*,
                       void*, nc_type);
static int nc3d_getvara(int ncid, int varid,
                       const size_t* , const size_t*,
                       void*, nc_type);
static int nc3d_getvars(int ncid, int varid,
		       const size_t*, const size_t*, const ptrdiff_t*,
                       void*, nc_type);
static int nc3d_getvar(int ncid, int varid, void* value, nc_type);


/* Local*/
static int initialized = 0;
static size_t zerostart[NC_MAX_DIMS];
static size_t singlecount[NC_MAX_DIMS];
static ptrdiff_t singlestride[NC_MAX_DIMS];

void
setnc3dispatch(int ncid)
{
    NC* ncp;
    NC_check_id(ncid,&ncp); /* get the NC structure*/
    ncp->dispatch = &netcdf3lib;
}


/* Following are dispatch entries for dap.
   The entries that do the real work have
   been moved to ncdap3.c
*/

int
nc__open_mp(const char *path, int mode, int basepe,
 	 size_t* chunksizehintp, int *ncidp)
{
    /* do some initialization*/
    if(!initialized) {
        int i;
        for(i=0;i<NC_MAX_DIMS;i++) {zerostart[i] = 0; singlecount[i] = 1; singlestride[i] = 1;}
	initialized = 1;
    }
    return THROW(nc3d_open(path, mode, ncidp));
}

int
nc__open(const char *path, int mode, 
 	 size_t* chunksizehintp, int *ncidp)
{
    return THROW(nc__open_mp(path, mode, 0, chunksizehintp, ncidp));
}

int
nc_open(const char *path, int mode, int *ncidp)
{
    return THROW(nc__open(path, mode, NULL, ncidp));
}

int
nc__create_mp(const char *path, int cmode, size_t initialsz, int basepe,
	      size_t* chunksizehintp, int *ncidp)
{
    int stat;
    stat = lnc3__create_mp(path,cmode,initialsz,basepe,chunksizehintp,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}

int
nc__create(const char *path, int cmode, size_t initialsz,
	    size_t* chunksizehintp, int *ncidp)
{
    int stat;
    stat = lnc3__create_mp(path,cmode,initialsz,0,chunksizehintp,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}

int
nc_create(const char *path, int cmode, int *ncidp)
{
    int stat;
    stat = lnc3__create_mp(path,cmode,0,0,NULL,ncidp);
    if(stat == NC_NOERR) setnc3dispatch(*ncidp);
    return THROW(stat);
}

/**************************************************/
/* Following are in the dispatch table */

int
nc3d_redef(int ncid)
{
    return THROW(NC_EPERM);
}

int
nc3d__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align)
{
    return THROW(NC_EPERM);
}

int
nc3d_enddef(int ncid)
{
    return THROW(NC_EPERM);
}

int
nc3d_sync(int ncid)
{
    return THROW(NC_EINVAL);
}

int
nc3d_abort(int ncid)
{
    return THROW(NC_NOERR);
}

/*
The DAP cdf info will have been integrated
into the existing netcdf data structures, so
that simple inquiries will work as expected
E.g all the nc_inq... entries (see dispatch.c)
*/

/*
The following entries are all ultimately mapped into a single
call that handles all cases.
*/

int
nc3d_get_var1(int ncid, int varid, const size_t* indexp, void* value)
{
    return THROW(nc3d_getvar1(ncid, varid, indexp, value, NC_NAT));
}

int
nc3d_get_vara(int ncid, int varid, const size_t* start, const size_t* count, void* value)
{
    return THROW(nc3d_getvara(ncid, varid, start, count, value, NC_NAT));
}

int
nc3d_get_vars(int ncid, int varid, const size_t* start, const size_t* count, const ptrdiff_t *stride,void*  value)
{
    return THROW(nc3d_getvars(ncid, varid, start, count, stride, value, NC_NAT));
}

/* Following is temporarily disabled */
int
nc3d_get_varm(int ncid, int varid, const size_t* start, const size_t* count, const ptrdiff_t *stride, const ptrdiff_t *imapp, void* value)
{
    return THROW(NC_EINVAL);
}

#define DEFGETVAR1(T1,T2,T3) \
int nc3d_get_var1_##T1(int ncid, int varid, const size_t*  indexp, T2* ip)\
{\
    int stat;\
    stat = nc3d_getvar1(ncid, varid, indexp, ip, T3); \
    return THROW(stat); \
}

DEFGETVAR1(text,char,NC_STRING)
DEFGETVAR1(uchar,unsigned char,NC_CHAR)
DEFGETVAR1(schar,signed char,NC_CHAR)
DEFGETVAR1(short,short,NC_SHORT)
DEFGETVAR1(int,int,NC_INT)
DEFGETVAR1(long,long,NC_INT)
DEFGETVAR1(float,float,NC_FLOAT)
DEFGETVAR1(double,double,NC_DOUBLE)

#define DEFGETVARA(T1,T2,T3) \
int nc3d_get_vara_##T1(int ncid, int varid, const size_t*  start,const size_t*  count, T2* ip)\
{\
    return THROW(nc3d_getvara(ncid, varid, start, count, ip, T3));\
}

DEFGETVARA(text,char,NC_STRING)
DEFGETVARA(uchar,unsigned char,NC_CHAR)
DEFGETVARA(schar,signed char,NC_CHAR)
DEFGETVARA(short,short,NC_SHORT)
DEFGETVARA(int,int,NC_INT)
DEFGETVARA(long,long,NC_INT)
DEFGETVARA(float,float,NC_FLOAT)
DEFGETVARA(double,double,NC_DOUBLE)

#define DEFGETVARS(T1,T2,T3) \
int nc3d_get_vars_##T1(int ncid, int varid, const size_t*  start,const size_t*  count, const ptrdiff_t* stridep, T2* ip)\
{\
    return THROW(nc3d_getvars(ncid, varid, start, count, stridep, ip, T3));\
}

DEFGETVARS(text,char,NC_STRING)
DEFGETVARS(uchar,unsigned char,NC_CHAR)
DEFGETVARS(schar,signed char,NC_CHAR)
DEFGETVARS(short,short,NC_SHORT)
DEFGETVARS(int,int,NC_INT)
DEFGETVARS(long,long,NC_INT)
DEFGETVARS(float,float,NC_FLOAT)
DEFGETVARS(double,double,NC_DOUBLE)

/* Following is temporarily disabled */

#define DEFGETVARM(T1,T2,T3) \
int nc3d_get_varm_##T1(int ncid, int varid, const size_t*  start,const size_t*  count, const ptrdiff_t* stridep, const ptrdiff_t* imapp, T2* ip)\
{\
    return THROW(NC_EINVAL);\
}

DEFGETVARM(text,char,NC_STRING)
DEFGETVARM(uchar,unsigned char,NC_CHAR)
DEFGETVARM(schar,signed char,NC_CHAR)
DEFGETVARM(short,short,NC_SHORT)
DEFGETVARM(int,int,NC_INT)
DEFGETVARM(long,long,NC_INT)
DEFGETVARM(float,float,NC_FLOAT)
DEFGETVARM(double,double,NC_DOUBLE)

#define DEFGETVAR(T1,T2,T3) \
int nc3d_get_var_##T1(int ncid, int varid, T2* ip)\
{\
    return THROW(nc3d_getvar(ncid,varid,ip,T3)); \
}

DEFGETVAR(text,char,NC_STRING)
DEFGETVAR(uchar,unsigned char,NC_CHAR)
DEFGETVAR(schar,signed char,NC_CHAR)
DEFGETVAR(short,short,NC_SHORT)
DEFGETVAR(int,int,NC_INT)
DEFGETVAR(long,long,NC_INT)
DEFGETVAR(float,float,NC_FLOAT)
DEFGETVAR(double,double,NC_DOUBLE)

/**********************************************/

static int
nc3d_getvar1(int ncid, int varid,
            const size_t* indexp,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,indexp,singlecount,singlestride,value,dsttype));
}

static int
nc3d_getvara(int ncid, int varid,
            const size_t* start, const size_t* count,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,start,count,singlestride,value,dsttype));
}

static int
nc3d_getvars(int ncid, int varid,
            const size_t* start, const size_t* count, const ptrdiff_t *stride,
	    void*  value,
	    nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,start,count,stride,value,dsttype));
}

static int
nc3d_getvar(int ncid, int varid,
            void* value,
            nc_type dsttype)
{
    /* utilize the more general varx routine*/
    return THROW(nc3d_getvarx(ncid,varid,zerostart,singlecount,singlestride,value,dsttype));
}
