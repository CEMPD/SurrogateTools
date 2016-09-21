/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCBYTES_H
#define OCBYTES_H 1

typedef struct OCbytes {
  int nonextendible; /* 1 => fail if an attempt is made to extend this buffer*/
  unsigned int alloc;
  unsigned int length;
  char* content;
} OCbytes;

#if defined(_CPLUSPLUS_) || defined(__CPLUSPLUS__) || defined(__CPLUSPLUS)
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

EXTERNC OCbytes* ocbnew(void);
EXTERNC void ocbfree(OCbytes*);
EXTERNC int ocbsetalloc(OCbytes*,unsigned int);
EXTERNC int ocbsetlength(OCbytes*,unsigned int);
EXTERNC int ocbfill(OCbytes*, char fill);

/* Produce a duplicate of the contents*/
EXTERNC char* ocbdup(OCbytes*);

/* Return the ith char; -1 if no such char */
EXTERNC int ocbget(OCbytes*,unsigned int);

/* Set the ith char */
EXTERNC int ocbset(OCbytes*,unsigned int,char);

EXTERNC int ocbappend(OCbytes*,char); /* Add at Tail */
EXTERNC int ocbappendn(OCbytes*,void*,unsigned int); /* Add at Tail */

EXTERNC int ocbcat(OCbytes*,char*);
EXTERNC int ocbsetcontents(OCbytes*, char*, unsigned int);

/* Following are always "in-lined"*/
#define ocblength(ocb) ((ocb)?(ocb)->length:0U)
#define ocballoc(ocb) ((ocb)?(ocb)->alloc:0U)
#define ocbcontents(ocb) ((ocb && ocb->content)?(ocb)->content:(char*)"")
#define ocbextend(ocb,len) ocbsetalloc((ocb),(len)+(ocb->alloc))
#define ocbclear(ocb) ((ocb)?(ocb)->length=0:0U)
#define ocbavail(ocb,n) ((ocb)?((ocb)->alloc - (ocb)->length) > (n):0U)

#endif /*OCBYTES_H*/
