/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdaperr.c,v 1.5 2009/03/25 01:48:31 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"

NCerror
ocerrtoncerr(OCerror ocerr)
{
    if(ocerr >= 0) return ocerr; /* really a system error*/
    switch (ocerr) {
    case OC_NOERR:	  return NC_NOERR;
    case OC_EBADID:	  return NC_EBADID;
    case OC_ECHAR:	  return NC_ECHAR;
    case OC_EDIMSIZE:	  return NC_EDIMSIZE;
    case OC_EEDGE:	  return NC_EEDGE;
    case OC_EINVAL:	  return NC_EINVAL;
    case OC_EINVALCOORDS: return NC_EINVALCOORDS;
    case OC_ENOMEM:	  return NC_ENOMEM;
    case OC_ENOTVAR:	  return NC_ENOTVAR;
    case OC_EPERM:	  return NC_EPERM;
    case OC_ESTRIDE:	  return NC_ESTRIDE;
    case OC_EDAP:	  return NC_EDAP;
    case OC_EXDR:	  return NC_EDAP;
    case OC_ECURL:	  return NC_ECURL;
    case OC_EBADURL:	  return NC_ECURL;
    case OC_EBADVAR:	  return NC_EDAP;
    case OC_EOPEN:	  return NC_EIO;
    case OC_EIO:	  return NC_EIO;
    default: break;
    }
    return NC_EDAP; /* default;*/
}
