/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdap3.h,v 1.10 2009/03/25 01:48:30 dmh Exp $
 *********************************************************************/
#ifndef NCDAP3_H
#define NCDAP3_H 1

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "netcdf.h"
#include "daprename.h"
#include "nc.h"

/* netcdf overrides*/
#include "dapnc.h"

#include "ncdap.h"


/* Import some internal procedures from libsrc*/

extern void drno_add_to_NCList(struct NC *ncp);
extern void drno_del_from_NCList(struct NC *ncp);
extern void drno_free_NC(struct NC *ncp);
extern struct NC* drno_new_NC(const size_t *chunkp);
extern void drno_set_numrecs(NC* ncp, size_t size);
extern size_t drno_get_numrecs(NC* ncp);
extern int drno_ncio_open(NC* ncp, const char* path, int mode);

#endif /*NCDAP3_H*/
