#ifndef NC_NCGEN_H
#define NC_NCGEN_H
/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/ncgen.h,v 1.10 2009/03/11 18:26:18 dmh Exp $
 *********************************************************************/

/* Local Configuration flags*/
#define ENABLE_C
#define ENABLE_CDF
#define INLINE_STRINGVAR

#include <config.h>

#include "netcdf.h"

#include "list.h"
#include "bytebuffer.h"

#ifdef USE_NETCDF4
#define CLASSICONLY 0
#else
#define CLASSICONLY 1
#endif

#define PANIC(msg) assert(panic(msg))
#define PANIC1(msg,a1) assert(panic(msg,a1))
#define PANIC2(msg,a1,a2) assert(panic(msg,a1,a2))
#define FPANIC(msg,a1) PANIC1(msg,a1)

#define MAX_NC_ATTSIZE    20000	/* max size of attribute (for ncgen) */
#define MAXTRST		  5000	/* max size of string value (for ncgen) */

/* Name of the root group,*/
/* although prefix construction*/
/* elides it.*/
#define ROOTGROUPNAME "root"

/* Define the possible classes of objects*/
/* extend the NC_XXX values*/
#define NC_GRP      100
#define NC_DIM      101
#define NC_VAR      102
#define NC_ATT      103
#define NC_TYPE     104
#define NC_ECONST   105
#define NC_FIELD    106
#define NC_ARRAY    107
#define NC_PRIM     108 /*Including NC_STRING */
#define NC_STRUCT  NC_COMPOUND /* alias */

#ifndef USE_NETCDF4
#define NC_CLASSIC_MODEL 1
#define NC_NETCDF4 2
/* Define what we would otherwise need from netcdf4's netcdf.h*/
typedef struct {
    size_t len; /* Length of VL data (in base type units) */
    void *p;    /* Pointer to VL data */
} nc_vlen_t;
#define	NC_UBYTE 	7	/* unsigned 1 byte int */
#define	NC_USHORT 	8	/* unsigned 2-byte int */
#define	NC_UINT 	9	/* unsigned 4-byte int */
#define	NC_INT64 	10	/* signed 8-byte int */
#define	NC_UINT64 	11	/* unsigned 8-byte int */
#define	NC_STRING 	12	/* string */
#define	NC_VLEN 	13	/* used internally for vlen types */
#define	NC_OPAQUE 	14	/* used internally for opaque types */
#define	NC_ENUM 	15	/* used internally for enum types */
#define	NC_COMPOUND 	16	/* used internally for compound types */

#define NC_MAX_BYTE 127
#define NC_MIN_BYTE (-NC_MAX_BYTE-1)
#define NC_MAX_CHAR 255
#define NC_MAX_SHORT 32767
#define NC_MIN_SHORT (-NC_MAX_SHORT - 1)
#define NC_MAX_INT 2147483647
#define NC_MIN_INT (-NC_MAX_INT - 1)
#define NC_MAX_FLOAT 3.402823466e+38f
#define NC_MIN_FLOAT (-NC_MAX_FLOAT)
#define NC_MAX_DOUBLE 1.7976931348623157e+308 
#define NC_MIN_DOUBLE (-NC_MAX_DOUBLE)
#define NC_MAX_UBYTE NC_MAX_CHAR
#define NC_MAX_USHORT 65535U
#define NC_MAX_UINT 4294967295U
#define NC_MAX_INT64 (9223372036854775807LL)
#define NC_MIN_INT64 (-9223372036854775807LL-1)
#define NC_MAX_UINT64 (18446744073709551615ULL)

#define NC_FILL_UBYTE   (255)
#define NC_FILL_USHORT  (65535)
#define NC_FILL_UINT    (4294967295U)
#define NC_FILL_INT64   ((long long)-9223372036854775806LL)
#define NC_FILL_UINT64  ((unsigned long long)18446744073709551614ULL)
#define NC_FILL_STRING  ""

#endif

/* Extend nc types with generic fill value*/
#define NC_FILLVALUE    31

/* nc_class is one of:
        NC_GRP NC_DIM NC_VAR NC_ATT NC_TYPE
*/
typedef nc_type nc_class;

/* nc_subclass is one of:
	NC_PRIM NC_OPAQUE NC_ENUM
	NC_FIELD NC_VLEN NC_COMPOUND
	NC_ECONST NC_ARRAY NC_FILLVALUE
*/
typedef nc_type nc_subclass;

/*
Define data structure
to hold special attribute values
for a given variable.
Global values are kept as
various C global variables
*/

/* Define a bit set for indicating which*/
/* specials were explicitly specified*/
#define _STORAGE_FLAG       0x01
#define _CHUNKSIZE_FLAG     0x02
#define _FLETCHER32_FLAG    0x04
#define _DEFLATE_FLAG       0x08
#define _SHUFFLE_FLAG       0x10
#define _ENDIAN_FLAG        0x20
#define _NOFILL_FLAG        0x40

/* Note: non-variable specials (e.g. _Format) are not included in this struct*/
typedef struct Specialdata {
    unsigned long    flags;
    struct Datalist* _Fillvalue;
    int   _Storage;      /* NC_CHUNKED | NC_CONTIGUOUS*/
    size_t*  _ChunkSizes;     /* NULL => defaults*/
        int nchunks;     /*  |_Chunksize| ; 0 => not specified*/
    int   _Fletcher32;     /* 1=>fletcher32*/
    int   _DeflateLevel; /* 0-9 => level*/
    int   _Shuffle;      /* 0 => false, 1 => true*/
    int   _Endianness;   /* 1 =>little, 2 => big*/
    int   _Fill ;        /* 0 => false, 1 => true WATCHOUT: this is inverse of NOFILL*/
} Specialdata;

/* Define a structure to hold*/
/* any one possible value*/
typedef union Constvalue {
    struct Datalist* compoundv; /* NC_COMPOUND*/
    char charv;                 /* NC_CHAR*/
    signed char int8v;          /* NC_BYTE*/
    unsigned char uint8v;       /* NC_UBYTE*/
    short int16v;               /* NC_SHORT*/
    unsigned short uint16v;     /* NC_USHORT*/
    int int32v;                 /* NC_INT*/
    unsigned int uint32v;       /* NC_UINT*/
    long long int64v;           /* NC_INT64*/
    unsigned long long uint64v; /* NC_UINT64*/
    float floatv;               /* NC_FLOAT*/
    double doublev;             /* NC_DOUBLE*/
    struct Stringv {		/* NC_STRING*/
	int len;
	char* stringv;
    } stringv;
    struct Opaquev {     /* NC_OPAQUE*/
	int len; /* length as originally written (rounded to even number)*/
	char* stringv; /*as  constant was written*/
		      /* (padded to even # chars >= 16)*/
		      /* without leading 0x*/
    } opaquev;
    struct Symbol* enumv;   /* NC_ECONST*/
} Constvalue;

/* Track a set of dimensions*/
/* (Note: the netcdf type system is deficient here)*/
typedef struct Dimset {
    int		     ndims;
    struct Symbol*   dimsyms[NC_MAX_DIMS]; /* Symbol for dimension*/
} Dimset;

typedef struct Constant {
    nc_type 	  nctype;
    int		  lineno;
    Constvalue    value;
} Constant;

typedef struct Diminfo {
    int   isconstant; /* separate constant from named dimension*/
    unsigned int  size; /* 0 => unlimited/unspecified*/
} Diminfo;

typedef struct Attrinfo {
    struct Symbol*   var; /* NULL => global*/
    unsigned long    count; /* number of instances (only valid after genxxx_datalist called*/
} Attrinfo;

typedef struct Typeinfo {
        struct Symbol*  basetype;
	int             hasvlen;  /* 1 => this type contains a vlen*/
	nc_type         typecode;
        unsigned long   size;     /* for opaque, compound, etc.*/
        unsigned long   offset;   /* fields in struct*/
        unsigned long   alignment;/* fields in struct*/
        Constant        econst;   /* for enum values*/
        Dimset          dimset;     /* for NC_VAR/NC_FIELD/NC_ATT*/
} Typeinfo;

typedef struct Varinfo {
    int		nattributes; /* |attributes|*/
    List*   attributes;  /* List<Symbol*>*/
    Specialdata special;
} Varinfo;

typedef struct Groupinfo {
    int is_root;
    struct Symbol* unlimiteddim;
} Groupinfo;

typedef struct Symbol {  /* symbol table entry*/
        struct Symbol*  next;    /* Linked list of all defined symbols*/
        nc_class        objectclass;  /* NC_DIM|NC_VLEN|NC_OPAQUE...*/
        nc_class        subclass;  /* NC_STRUCT|...*/
        char*           name;
        struct Symbol*  ref;  /* ptr to the symbol if is_ref is true*/
        struct Symbol*  container;  /* The group containing this symbol.*/
				    /* for fields or enumids, it is*/
				    /* the parent type.*/
	struct Symbol*  location;   /* current group when symbol was created*/
	List*       subnodes;  /* sublist for enum or struct or group*/
	int             is_prefixed; /* prefix was specified (vs computed).*/
        List*       prefix;  /* List<Symbol*>*/
        struct Datalist* data; /* shared by variables and attributes*/
	/* Note that we cannot union these because some kinds of symbols*/
        /* use more than one part*/
        Typeinfo  typ; /* type info for e.g. var, att, etc.*/
        Varinfo   var;
        Attrinfo  att;        
        Diminfo   dim;
        Groupinfo grp;
	/* Misc pieces of info*/
	int             lineno;  /* at point of creation*/
	int		touched; /* for sorting*/
        int             is_ref;  /* separate name defs  from refs*/
	char*           lname; /* cached C or FORTRAN name*/
        int             ncid;  /* from netcdf API: varid, or dimid, or etc.*/
} Symbol;

/* Define a structure to track*/
/* location of current read point in the Datalist sequence*/
/* In effect, we are parsing the data sequence.*/
/* Push and pop of data sources is supported (see srcpush() below).*/

typedef struct Datasrc {
#ifdef DSPTR
    Constant* next; /* data<=next<last*/
    Constant* last;
#else
    unsigned int index;     /* 0..length-1*/
    unsigned int length;
#endif
    int          autopop;   /* pop when at end*/
    Constant*    data;      /* duplicate pointer; so do not free.*/
    struct Datasrc* stack;
} Datasrc;

#endif /*!NC_NCGEN_H*/
