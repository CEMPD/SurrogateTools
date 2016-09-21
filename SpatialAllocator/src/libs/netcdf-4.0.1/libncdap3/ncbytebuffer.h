/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef NCBYTEBUFFER_H
#define NCBYTEBUFFER_H 1

typedef struct NCbytebuffer {
  int nonextendible; /* 1 => fail if an attempt is made to extend this buffer*/
  unsigned int alloc;
  unsigned int length;
  char* content;
} NCbytebuffer;

#if defined(_CPLUSPLUS_) || defined(__CPLUSPLUS__) || defined(__CPLUSPLUS)
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

EXTERNC NCbytebuffer* ncbytesnew(void);
EXTERNC void ncbytesfree(NCbytebuffer*);
EXTERNC int ncbytessetalloc(NCbytebuffer*,unsigned int);
EXTERNC int ncbytessetlength(NCbytebuffer*,unsigned int);
EXTERNC int ncbytesfill(NCbytebuffer*, char fill);

/* Produce a duplicate of the contents*/
EXTERNC char* ncbytesdup(NCbytebuffer*);

/* Return the ith char; -1 if no such char */
EXTERNC int ncbytesget(NCbytebuffer*,unsigned int);

/* Set the ith char */
EXTERNC int ncbytesset(NCbytebuffer*,unsigned int,char);

EXTERNC int ncbytesappend(NCbytebuffer*,char); /* Add at Tail */
EXTERNC int ncbytesappendn(NCbytebuffer*,void*,unsigned int); /* Add at Tail */

EXTERNC int ncbytescat(NCbytebuffer*,char*);
EXTERNC int ncbytessetcontents(NCbytebuffer*, char*, unsigned int);

/* Following are always "in-lined"*/
#define ncbyteslength(ocb) ((ocb)?(ocb)->length:0U)
#define ncbytesalloc(ocb) ((ocb)?(ocb)->alloc:0U)
#define ncbytescontents(ocb) ((ocb && ocb->content)?(ocb)->content:(char*)"")
#define ncbytesextend(ocb,len) ncbytessetalloc((ocb),(len)+(ocb->alloc))
#define ncbytesclear(ocb) ((ocb)?(ocb)->length=0:0U)
#define ncbytesavail(ocb,n) ((ocb)?((ocb)->alloc - (ocb)->length) > (n):0U)

#endif /*NCBYTEBUFFER_H*/
