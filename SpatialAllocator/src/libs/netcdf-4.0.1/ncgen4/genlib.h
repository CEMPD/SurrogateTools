#ifndef NC_GENLIB_H
#define NC_GENLIB_H
/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/genlib.h,v 1.5 2009/03/11 18:26:18 dmh Exp $
 *********************************************************************/
#include <stdlib.h>
#include <limits.h>
#include "generr.h"

/* break if C Line length exceeds this*/
#define C_MAX_STMT	72

#define PATHSEPARATOR   "/"

/* Convenience*/
#define REQUIRED 1
#define DONTCARE -1
#define NOTFLAT 0

#define nulldup(s) ((s)==NULL?NULL:strdup(s))
#define nulllen(s) ((s)==NULL?0:strlen(s))


extern void derror ( const char *fmt, ... )
#ifdef _GNUC_
       __attribute__ ((format (printf, 1, 2)))
#endif
;
extern void	verror ( const char *fmt, ... )
#ifdef _GNUC_
       __attribute__ ((format (printf, 1, 2)))
#endif
;

/*
All external procedures in ncgen.h have been moved to this file.
*/

/* from: genlib.c */

extern void define_netcdf(void);/* generates all define mode stuff */
extern void close_netcdf ( void ); /* generates close */

/* from: escapes.c */
extern void expand_escapes ( char* termstring, char* yytext, int yyleng );
extern void deescapify(char *name); /* redunandt over expand_escapes?*/
extern char* decodify(const char *name);
extern char* escapifychar(int,char*,int);
extern char* escapify(char*,int,size_t);

/* from: getfill.c */
extern void nc_getfill(Constant*);
extern char* nc_dfaltfillname(nc_type);
extern struct Datalist* getfiller(Symbol*); /* type or variable*/
extern int checkfillvalue(Symbol*, struct Datalist*);

/* from: ncgen.y */
extern Symbol* install(const char *sname);
extern Symbol* basetypefor(nc_type nctype);/* Convert nctype to a Symbol*/
extern Symbol* makearraytype(Symbol*, Dimset*);

/* from: cvt.c */
extern void convert1(Constant*,Constant*); /* Convert an arbitrary value to another */
extern void setprimlength(Constant* prim, unsigned long len);
extern struct Datalist* convertstringtochars(Constant* str);


/* from: semantic.c */
extern  void processsemantics();
extern  size_t nctypesize(nc_type);
extern  Symbol* locate(Symbol* refsym);
extern  Symbol* lookup(nc_class objectclass, Symbol* pattern);
extern  Symbol* lookupingroup(nc_class objectclass, char* name, Symbol* grp);
extern  Symbol* lookupgroup(List* prefix);
#ifndef NO_STDARG
extern  void semerror(const int, const char *fmt, ...);
#else
extern  void semerror(lno,fmt,va_alist) const int lno; const char* fmt; va_dcl;
#endif

/* Generators for cdf, c, and fortran */
/* from: gencdf.c */
extern void gen_netcdf(const char *filename);
extern void cl_netcdf(void);

/* from: genc.c */
extern void gen_ncc(const char *filename);
extern void cl_c(void);
extern const char* cname(Symbol*);
extern const char* ctypename(Symbol*);
extern void cprint(Bytebuffer* buf);
extern void cpartial(char*);
extern void cline(char*);
extern void clined(int,char*);
extern void flushcode(void);
extern char* indented(int n);
#ifdef DEBUG
extern void report(char* lead, Datalist* list);
#endif

/* from: main.c */
extern int usingclassic;

/* Global data */

extern Symbol* symlist;      /* all symbol objects created */
extern Symbol* rootgroup;

/* Track definitions of dims, types, attributes, and vars*/
extern List* grpdefs;
extern List* dimdefs;
extern List* attdefs;
extern List* gattdefs;
extern List* xattdefs;
extern List* typdefs;
extern List* vardefs;
extern List* condefs;

extern int CDFmodel;

extern int lineno;
extern int derror_count;
extern int kflag_flag;
extern int cmode_modifier;
extern int netcdf_flag;
extern int c_flag;
extern int fortran_flag;
extern int nofill_flag;
extern char structformat_flag;
extern char* mainname;

extern char* progname; /* for error messages*/
extern char *netcdf_name; /* command line -o file name */
extern char *datasetname; /* name from the netcdf <name> {} */
extern char *cdlname; /* name from the command line */

/* from: util.c */
extern void* emalloc (size_t);
extern void* ecalloc (size_t);
extern void* erealloc(void*,size_t);

#endif /*!NC_GENLIB_H*/
