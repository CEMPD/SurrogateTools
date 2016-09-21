/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCNODE_H
#define OCNODE_H

/*! Specifies the OCtype.*/
/* Enum of primitive and non-primitive type tags.*/
/* Primtives = Duplicate of the NODE_Byte..Node_URL union nc_type*/
typedef enum OCtype {
    /* Primitives*/
    OC_NAT     = 0,
    OC_Char    = 1,
    OC_Byte    = 2,
    OC_UByte   = 3,
    OC_Int16   = 4,
    OC_UInt16  = 5,
    OC_Int32   = 6,
    OC_UInt32  = 7,
    OC_Int64   = 8,
    OC_UInt64  = 9,
    OC_Float32 = 10,
    OC_Float64 = 11,
    OC_String  = 12,
    OC_URL     = 13,
    /* Non-primitives*/
    OC_Dataset      = 100,
    OC_Sequence     = 101,
    OC_Grid         = 102,
    OC_Structure    = 103,
    OC_Dimension    = 104,
    OC_Attribute    = 105,
    OC_Attributeset = 106,
    OC_Primitive    = 107
} OCtype;

#define MAXDAPPRIM OC_URL

/*! Specifies the Diminfo. */
/* Track info purely about declared dimensions*/
/* More information is included in the Dimdata structure (dim.h)*/
typedef struct Diminfo {
    struct OCnode* array;   /* defining array node (if known)*/
    unsigned int arrayindex;	    /* which rank position is this dimension in the array*/
    size_t declsize;	    /* from DDS*/
} Diminfo;

/*! Specifies the Arrayinfo.*/
typedef struct Arrayinfo {
    /* The complete set of dimension info applicable to this node*/
    OClist*  dimensions;
    /* convenience (because they are computed so often*/
    unsigned int rank; /* == |dimensions|*/
} Arrayinfo;

/*! Specifies the Attribute.*/
/* This is the compiled form of attributes*/
typedef struct Attribute {
    char*   name;
    OCtype etype; /* type of the attribute */
    size_t  nvalues;
    char*   values;  /* |values| = nvalues*nctypesize(nctype)*/
} Attribute;

/*! Specifies the Attinfo.*/
/* This is the form as it comes out of the DAS parser*/
typedef struct Attinfo {
    int isglobal;   /* is this supposed to be a global attribute set?*/
    OClist* values; /* oclist<char*>*/
} Attinfo;

/*! Specifies the OCnode. */
typedef struct OCnode {
    OCtype	    octype;
    OCtype          etype; /* essentially the dap type from the dds*/
    char*           name;
    char*           fullname;
    struct OCnode*  container;
    Diminfo         dim; /* octype == OC_Dimension*/
    /* 1/21/08: James says grids and sequences cannot have dimensions*/
    Arrayinfo       array; /* octype == {OC_Structure, OC_Primitive}*/
    Attinfo att;
    /* primary edge info*/
    OClist* subnodes; /*oclist<OCnode*>*/
    int       attributed; /* 1 if merge was done*/
    OClist* attributes; /* oclist<Attribute*>*/
    /* Size of one element: size == 0 => not uniform*/
    size_t xdrinstancesize; /* includes array overhead of subnodes, but not of this node*/
    size_t xdrdimsize; /* instancesize + array overhead*/
    struct OCconnection* owner; /* To which connection do we belong*/
    int    datadds; /* if octype == OC_Dataset, did this come from a datadds (vs a dds).*/
    void* public; /* for use by idl client only: temporary*/
} OCnode;

#endif /*OCNODE_H*/
