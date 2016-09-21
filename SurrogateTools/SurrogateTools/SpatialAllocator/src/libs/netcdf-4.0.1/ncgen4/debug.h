#ifndef NCGEN_DEBUG_H
#define NCGEN_DEBUG_H

/*#define F*/

/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/debug.h,v 1.5 2009/03/11 18:26:17 dmh Exp $
 *********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "generr.h"
#include "bytebuffer.h"

extern int yydebug;
extern int debug;

extern void fdebug(const char *fmt, ...);

extern void dumptransform(Datalist*);
extern void dumpdatalist(Datalist*,char*);
extern void bufdump(Datalist*,Bytebuffer*);
extern void dumpgroup(Symbol* g);
extern void dumpsrc(Datasrc*,char*);

#ifdef F
#define DUMPSRC(src,tag) dumpsrc(src,tag)
#else
#define DUMPSRC(src,tag)
#endif

#endif /*NCGEN_DEBUG_H*/
