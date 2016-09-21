/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dispatch3.c,v 1.8 2009/03/25 01:48:29 dmh Exp $
 *********************************************************************/
#include "config.h"

#include "daprename.h"

#include "ncdap3.h"

#include "dispatch3.h"
#include "dapdispatch3.h"

NC_Dispatch3 netcdf3lib = {
lnc3_close,
lnc3_sync,
lnc3_abort,
lnc3_redef,
lnc3__enddef,
lnc3_enddef,
lnc3_get_var1,
lnc3_get_var1_text,
lnc3_get_var1_uchar,
lnc3_get_var1_schar,
lnc3_get_var1_short,
lnc3_get_var1_int,
lnc3_get_var1_long,
lnc3_get_var1_float,
lnc3_get_var1_double,
lnc3_get_vara,
lnc3_get_vara_text,
lnc3_get_vara_uchar,
lnc3_get_vara_schar,
lnc3_get_vara_short,
lnc3_get_vara_int,
lnc3_get_vara_long,
lnc3_get_vara_float,
lnc3_get_vara_double,
lnc3_get_vars,
lnc3_get_vars_text,
lnc3_get_vars_uchar,
lnc3_get_vars_schar,
lnc3_get_vars_short,
lnc3_get_vars_int,
lnc3_get_vars_long,
lnc3_get_vars_float,
lnc3_get_vars_double,
lnc3_get_varm,
lnc3_get_varm_text,
lnc3_get_varm_uchar,
lnc3_get_varm_schar,
lnc3_get_varm_short,
lnc3_get_varm_int,
lnc3_get_varm_long,
lnc3_get_varm_float,
lnc3_get_varm_double,
/*lnc3_get_var,*/
lnc3_get_var_text,
lnc3_get_var_uchar,
lnc3_get_var_schar,
lnc3_get_var_short,
lnc3_get_var_int,
lnc3_get_var_long,
lnc3_get_var_float,
lnc3_get_var_double,
};

NC_Dispatch3 ncdap3lib = {
nc3d_close,
nc3d_sync,
nc3d_abort,
nc3d_redef,
nc3d__enddef,
nc3d_enddef,
nc3d_get_var1,
nc3d_get_var1_text,
nc3d_get_var1_uchar,
nc3d_get_var1_schar,
nc3d_get_var1_short,
nc3d_get_var1_int,
nc3d_get_var1_long,
nc3d_get_var1_float,
nc3d_get_var1_double,
nc3d_get_vara,
nc3d_get_vara_text,
nc3d_get_vara_uchar,
nc3d_get_vara_schar,
nc3d_get_vara_short,
nc3d_get_vara_int,
nc3d_get_vara_long,
nc3d_get_vara_float,
nc3d_get_vara_double,
nc3d_get_vars,
nc3d_get_vars_text,
nc3d_get_vars_uchar,
nc3d_get_vars_schar,
nc3d_get_vars_short,
nc3d_get_vars_int,
nc3d_get_vars_long,
nc3d_get_vars_float,
nc3d_get_vars_double,
nc3d_get_varm,
nc3d_get_varm_text,
nc3d_get_varm_uchar,
nc3d_get_varm_schar,
nc3d_get_varm_short,
nc3d_get_varm_int,
nc3d_get_varm_long,
nc3d_get_varm_float,
nc3d_get_varm_double,
/*lnc3_get_var,*/
nc3d_get_var_text,
nc3d_get_var_uchar,
nc3d_get_var_schar,
nc3d_get_var_short,
nc3d_get_var_int,
nc3d_get_var_long,
nc3d_get_var_float,
nc3d_get_var_double,
};

int
nc_redef(int ncid)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->redef(ncid);
}

int
nc__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->_enddef(ncid,h_minfree,v_align,v_minfree,r_align);
}

int
nc_enddef(int ncid)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->enddef(ncid);
}

int
nc_sync(int ncid)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->sync(ncid);
}

int
nc_abort(int ncid)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->abort(ncid);
}

int
nc_close(int ncid)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->close(ncid);
}

int
nc_get_var1(int ncid, int varid, const size_t *indexp, void *value)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1(ncid,varid,indexp, value);
}

int
nc_get_var1_text(int ncid, int varid, const size_t *indexp, char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_text(ncid,varid,indexp,ip);
}

int
nc_get_var1_uchar(int ncid, int varid, const size_t *indexp, unsigned char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_uchar(ncid,varid,indexp,ip);
}

int
nc_get_var1_schar(int ncid, int varid, const size_t *indexp, signed char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_schar(ncid,varid,indexp,ip);
}

int
nc_get_var1_short(int ncid, int varid, const size_t *indexp, short *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_short(ncid,varid,indexp,ip);
}

int
nc_get_var1_int(int ncid, int varid, const size_t *indexp, int *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_int(ncid,varid,indexp,ip);
}

int
nc_get_var1_long(int ncid, int varid, const size_t *indexp, long *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_long(ncid,varid,indexp,ip);
}

int
nc_get_var1_float(int ncid, int varid, const size_t *indexp, float *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_float(ncid,varid,indexp,ip);
}

int
nc_get_var1_double(int ncid, int varid, const size_t *indexp, double *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var1_double(ncid,varid,indexp,ip);
}

int
nc_get_vara(int ncid, int varid, const size_t *start, const size_t *count, void *value)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara(ncid,varid, start,count, value);
}

int
nc_get_vara_text(int ncid, int varid, const size_t *startp, const size_t *countp, char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_text(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, unsigned char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_uchar(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_schar(int ncid, int varid, const size_t *startp, const size_t *countp, signed char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_schar(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_short(int ncid, int varid, const size_t *startp, const size_t *countp, short *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_short(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_int(int ncid, int varid, const size_t *startp, const size_t *countp, int *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_int(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_long(int ncid, int varid, const size_t *startp, const size_t *countp, long *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_long(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_float(int ncid, int varid, const size_t *startp, const size_t *countp, float *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_float(ncid,varid, startp,countp,ip);
}

int
nc_get_vara_double(int ncid, int varid, const size_t *startp, const size_t *countp, double *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vara_double(ncid,varid, startp,countp,ip);
}

int
nc_get_vars(int ncid, int varid, const size_t *start, const size_t *count, const ptrdiff_t *stride,void * value)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars(ncid,varid, start,count, stride, value);
}

int
nc_get_vars_text(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_text(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,unsigned char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_uchar(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_schar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,signed char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_schar(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_short(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,short *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_short(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_int(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,int *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_int(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_long(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,long *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_long(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_float(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,float *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_float(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_vars_double(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,double *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_vars_double(ncid,varid, startp,countp, stridep,ip);
}

int
nc_get_varm(int ncid, int varid, const size_t *start, const size_t *count, const ptrdiff_t *stride, const ptrdiff_t *imapp, void *value)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm(ncid,varid, start,count, stride, imapp, value);
}

int
nc_get_varm_text(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_text(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_uchar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, unsigned char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_uchar(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_schar(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, signed char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_schar(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_short(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, short *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_short(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_int(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, int *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_int(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_long(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, long *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_long(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_float(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t *imapp, float *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_float(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_varm_double(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep,const ptrdiff_t * imapp, double *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_varm_double(ncid,varid, startp,countp, stridep,imapp,ip);
}

int
nc_get_var_text(int ncid, int varid, char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_text(ncid,varid,ip);
}

int
nc_get_var_uchar(int ncid, int varid, unsigned char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_uchar(ncid,varid,ip);
}

int
nc_get_var_schar(int ncid, int varid, signed char *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_schar(ncid,varid,ip);
}

int
nc_get_var_short(int ncid, int varid, short *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_short(ncid,varid,ip);
}

int
nc_get_var_int(int ncid, int varid, int *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_int(ncid,varid,ip);
}

int
nc_get_var_long(int ncid, int varid, long *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_long(ncid,varid,ip);
}

int
nc_get_var_float(int ncid, int varid, float *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_float(ncid,varid,ip);
}

int
nc_get_var_double(int ncid, int varid, double *ip)
{
    NC* ncp;
    int stat = NC_check_id(ncid, &ncp);
    if(stat != NC_NOERR) return stat;
    return ncp->dispatch->get_var_double(ncid,varid,ip);
}
