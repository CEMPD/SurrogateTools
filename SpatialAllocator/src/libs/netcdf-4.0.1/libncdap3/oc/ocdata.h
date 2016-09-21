/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCDATA_H
#define OCDATA_H

typedef enum OCmode {
    Emptymode =0, /* => unused node in the list of connections*/
    Nullmode  =1, /* => node not currently associated with any data*/
    Dimmode   =2,
    Recordmode=3,
    Fieldmode =4,
    Datamode  =5
} OCmode;

typedef struct Dimcounter {
    int rank;
    size_t index[OC_MAX_DIMS];
    size_t size[OC_MAX_DIMS];
} Dimcounter;

extern const char StartOfoclist;
extern const char EndOfoclist;

/* Skip arbitrary dimensioned instance; Handles dimensioning.*/
extern int oc_skip(OCnode* node, XDR* xdrs);

/* Skip arbitrary single instance; except for primitives
   Assumes that parent will handle arrays of compound instances
   or records of compound instances of this node type*/
extern int oc_skipinstance(OCnode* node, XDR* xdrs);

extern int oc_countrecords(OCnode* node, XDR* xdrs, size_t* nrecordsp);

extern int oc_xdrread(XDR*, char* memory, size_t, int packed, OCtype, unsigned int index, size_t count);

#endif /*OCDATA_H*/
