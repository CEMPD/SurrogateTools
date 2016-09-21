/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCINTERNAL_H
#define OCINTERNAL_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <stdarg.h>
#include <curl/curl.h>
#include "oclist.h"
#include "ocbytes.h"

#ifdef OCCACHEPOS
extern void ocxdrstdio_create(XDR*,FILE*,enum xdr_op);
#else
#define ocxdrstdio_create(xdrs,file,op) xdrstdio_create(xdrs,file,op)
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "oc.h"
#include "ocdatatypes.h"
#include "constraints.h"
#include "ocnode.h"
#include "dapurl.h"
#include "ocutil.h"
#include "oclog.h"

#define nulldup(s) (s==NULL?NULL:strdup(s))
#define nullstring(s) (s==NULL?"(null)":s)

#define PATHSEPARATOR "."

/*! Specifies all the info about a particular DAP object*/
typedef struct OCDXD {
    DAPURL url;
    char* text;
    OCnode* tree;
    OClist* nodes; /* all nodes in tree*/
} OCDXD;

/*! Specifies the DAP packet info.*/
typedef struct OCDAP {
    char* originalurl;
    DAPURL url;
    OCbytes* packet;
    OClist* projections; /* oclist<ProjectionClause*>*/
    CURL* curl; /* curl handle*/
    unsigned long bod;
    char* filename;
    FILE* file;
    unsigned long filesize;
    XDR* xdrs;
} OCDAP;

/*! Specifies the OCSchema.*/
typedef struct OCSCHEMA {
    OCDXD das;
    OCDXD dds;
    OCDXD datadds;
} OCSCHEMA;

/*! Specifies the OCMap.*/
typedef struct OCMAP {
    int       uniform; /* 1 => all instances same size == sizes[0]*/
    OClist* sizes; /* sizes for each instance */
} OCMAP;

/*! Specifies the OCContent. */
typedef struct OCCONTENT {
    struct OCcontent* contentlist;
    struct OCmemdata* memdata; /* !NULL iff compiled */
} OCCONTENT;

/*! Specifies the OCconnection. */
typedef struct OCconnection {
    OCSCHEMA       schema;
    OCDAP          dap;
    OCMAP          map;
    OCCONTENT      content;
/*    void*          private;*/
} OCconnection;

/*
Watch out: this data structures uses the standard C trick of
casting a long memory chunk to be an instance of this
object, which means that the data part may actually be
longer than 8 chars.
*/
typedef struct OCmemdata {
    unsigned long octype; /* Actually instance of OCtype, but guaranteed to be |long| */
    unsigned long etype; /* Actually instance of OCtype, but guaranteed to be |long| */
    unsigned long mode; /* Actually instance of OCmode, but guaranteed to be |long| */
    unsigned long count; /* count*octypesize(datatype) == |data| */
    union {
        struct OCmemdata* mdata[2];
        unsigned int* idata[2];
        char data[8]; /* Actually prefix of the data; want to start on longlong boundary */
    } data;
} OCmemdata;


/* (Almost) All shared procedure definitions are kept here
   except for: ocdebug.h ocutil.h
   The true external interfac is defined in oc.h
   Location: ocnode.c*/
extern OCnode* makesequencedimension(OCnode*);
extern OCnode* makepseudodimension(size_t size, OCnode* array, int index);
extern OCnode* makeocnode(char* name, OCtype ptype, OCconnection*);
extern void collectpathtonode(OCnode* node, OClist* path);
extern void computefullnames(OCnode* root);
extern void computeocsemantics(OClist*);
extern void addattribute(Attribute* attr, OCnode* parent);
extern Attribute* makeattribute(char* name, OCtype ptype, OClist* values);
extern size_t ocsetsize(OCnode* node);
extern void ocfreenodes(OClist*);

/* Location: constraints.c*/
extern int assignprojectionsource(OClist*);
extern int validateprojections(OClist*);

/* Location: dapparselex.c*/
extern int dapdebug;
extern OCnode* DAPparse(OCconnection*, char* packet, OClist* cdfnodes);
extern char* dimnameanon(char* basename, unsigned int index);

/* Location: ceparselex.c*/
extern int cedebug;
extern OClist* CEparse(OCconnection*,char* input);

/* Location: occompile.c*/
extern void freeocmemdata(OCmemdata* md);

#endif /*COMMON_H*/
