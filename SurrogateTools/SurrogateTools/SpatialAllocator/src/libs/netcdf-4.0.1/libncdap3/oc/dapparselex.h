/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef DAPPARSELEX_H
#define DAPPARSELEX_H 1

typedef void* Object;

/* Need this decl from .y file*/
#define YYSTYPE Object

/*! Specifies Attrvalue. */
typedef union Attrvalue {
    signed char bytev;      /* OC_BYTE*/
    signed short int16v;    /* OC_SHORT*/
    unsigned short uint16v; /* OC_USHORT*/
    int int32v;             /* OC_INT*/
    unsigned int uint32v;   /* OC_UINT*/
    float float32v;         /* OC_FLOAT*/
    double float64v;        /* OC_DOUBLE*/
    char* stringv;          /* OC_String*/
    char* urlv;             /* OC_URL*/
} Attrvalue;

/*! Specifies the Lasttoken. */
typedef struct Lasttoken {
    char* text;
    int token;
} Lasttoken;

/*! Specifies the Lexstate. */
typedef struct Lexstate {
    char* input;
    char* next; /* next char in uri.query*/
    OCbytes* yytext;
    int lineno;
    Lasttoken lasttoken;
    OClist* reclaim; /* for reclaiming token strings*/
} Lexstate;

/*! Specifies the DAPparsestate. */
typedef struct DAPparsestate {
    struct OCnode* result;
    Lexstate lexstate;
    OClist* ocnodes;
    OCconnection* conn;
} DAPparsestate;

extern void dap_parse_error(DAPparsestate*,const char *fmt, ...);

extern Object datasetbody(DAPparsestate*,Object decls, Object name);
extern Object declarations(DAPparsestate*,Object decls, Object decl);
extern Object arraydecls(DAPparsestate*,Object arraydecls, Object arraydecl);
extern Object arraydecl(DAPparsestate*,Object name, Object size);
extern Object attributebody(DAPparsestate*,Object attrlist);
extern Object attrlist(DAPparsestate*,Object attrlist, Object attrtuple);
extern Object attribute(DAPparsestate*,Object name, Object value, Object etype);
extern Object attrset(DAPparsestate*,Object name, Object attributes);
extern Object attrvalue(DAPparsestate*,Object valuelist, Object value, Object etype);

extern Object makebase(DAPparsestate*,Object name, Object etype, Object dimensions);
extern Object makestructure(DAPparsestate*,Object name, Object dimensions, Object fields);
extern Object makesequence(DAPparsestate*,Object name, Object members);
extern Object makegrid(DAPparsestate*,Object name, Object arraydecl, Object mapdecls);

#endif /*DAPPARSELEX_H*/
