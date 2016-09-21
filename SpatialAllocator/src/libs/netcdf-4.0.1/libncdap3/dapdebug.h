/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdebug.h,v 1.5 2009/03/30 15:47:25 dmh Exp $
 *********************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

#define PANIC(msg) assert(dappanic(msg));
#define PANIC1(msg,arg) assert(dappanic(msg,arg));
#define PANIC2(msg,arg1,arg2) assert(dappanic(msg,arg1,arg2));

#define ASSERT(expr) if(!(expr)) {PANIC(#expr);} else {}

extern int ncdap3debug;

extern int dappanic(const char* fmt, ...);

/*
Provide wrapped versions of calloc and malloc.
The wrapped version panics if memory
is exhausted.  It also guarantees that the
memory has been zero'd.
*/

#define ecalloc(x,y) dapcalloc(x,y)
#define emalloc(x)   dapcalloc(x,1)
#define efree(x) dapfree(x)
extern void* dapcalloc(size_t size, size_t nelems);
extern void* dapmalloc(size_t size);
extern void  dapfree(void*);

#define MEMCHECK(var,throw) {if((var)==NULL) return (throw);}

#undef CATCHERROR

#ifdef CATCHERROR
#define THROW(e) dapthrow(e)
/* Place breakpoint on dapbreakpoint to catch errors close to where they occur*/
extern int dapbreakpoint(int err);
extern int dapthrow(int err);
#else
#define THROW(e) (e)
#endif

#endif /*DEBUG_H*/

