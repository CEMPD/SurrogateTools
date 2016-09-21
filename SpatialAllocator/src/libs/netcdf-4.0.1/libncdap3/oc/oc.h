/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OC_H
#define OC_H

/* OC_MAX_DIMS should be greater or equal to max allowed by dap or netcdf*/
#define OC_MAX_DIMS 1024

#define OC_UNDEFINED ((unsigned long)0xffffffffffffffffULL)

/* Tag sequence dimensions length*/
#define OC_VLEN 0

/* Define a set of error
  positive are system errors
  (needs work)*/

typedef int OCerror;
#define OC_NOERR	(0)
#define OC_EBADID	(-1)
#define OC_ECHAR	(-2)
#define OC_EDIMSIZE	(-3)
#define OC_EEDGE	(-4)
#define OC_EINVAL	(-5)
#define OC_EINVALCOORDS	(-6)
#define OC_ENOMEM	(-7)
#define OC_ENOTVAR	(-8)
#define OC_EPERM	(-9)
#define OC_ESTRIDE	(-10)
#define OC_EDAP		(-11)
#define OC_EXDR		(-12)
#define OC_ECURL	(-13)
#define OC_EBADURL	(-14)
#define OC_EBADVAR	(-15)
#define OC_EOPEN	(-16)
#define OC_EIO   	(-17)

/* Define the effective API*/

struct OCconnection; /* forward*/
struct OCwalker; /* forward*/

extern int oc_open(struct OCconnection**);
extern void oc_close(struct OCconnection* state);

extern int oc_fetchdas(struct OCconnection*, const char* url);
extern int oc_fetchdds(struct OCconnection*, const char* url);
extern int oc_fetchdatadds(struct OCconnection*, const char* url);

extern void oc_ddsclear(struct OCconnection*);
extern void oc_dasclear(struct OCconnection*);
extern void oc_dataddsclear(struct OCconnection*);

extern struct OCnode* oc_getdds(struct OCconnection*);
extern struct OCnode* oc_getdas(struct OCconnection*);
extern struct OCnode* oc_getdatadds(struct OCconnection*);

extern OClist* oc_getddsall(struct OCconnection*);
extern OClist* oc_getdasall(struct OCconnection*);
extern OClist* oc_getdataddsall(struct OCconnection*);

extern const char* oc_getddstext(struct OCconnection*);
extern const char* oc_getdastext(struct OCconnection*);
extern const char* oc_getdataddstext(struct OCconnection*);

/* Merge DAS with DDS or DATADDS*/

/* Mnemonic flags*/
#define MERGEDDS 0
#define MERGEDATADDS 1

extern int oc_ddsdasmerge(struct OCconnection*, int whichdds);

extern int oc_compile(struct OCconnection* state);
extern int oc_compile(struct OCconnection* state);

#endif /*OC_H*/
