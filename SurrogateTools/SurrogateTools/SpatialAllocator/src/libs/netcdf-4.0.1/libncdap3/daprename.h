/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/daprename.h,v 1.3 2009/03/11 20:54:21 dmh Exp $
 *********************************************************************/
/*
Define rename criteris
NETCDF4=no  => use netcdf.h
NETCDF4=yes => use netcdf3.h then nctolnc3.h
where
netcdf3l.h is netcdf3 with nc3_* converted to lnc3_*
nctonc3.h converts nc_* to nc3_

Note that this is tricky because DAP will be
taking over only selected API procedures such
as nc_open or nc_close.  This means that we
only want to rename those procedures
when dap is true.  That is why e.g. the nc3tolnc3.h files
and the netcdf3l.h file only define and rename
those selected procedures.
See also libsrc/rename.h
*/

#ifdef USE_NETCDF4
# include "netcdf3.h"
# include "netcdf3l.h"
# include "nctonc3.h"
#else
# include "netcdf.h"
# include "netcdf3l.h"
#endif
