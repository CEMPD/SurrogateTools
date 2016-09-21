/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H 1

typedef struct Bytebuffer {
  int nonextendible; /* 1 => fail if an attempt is made to extend this buffer*/
  unsigned int alloc;
  unsigned int length;
  char* content;
} Bytebuffer;

#if defined(_CPLUSPLUS_) || defined(__CPLUSPLUS__) || defined(__CPLUSPLUS)
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

EXTERNC Bytebuffer* bbNew(void);
EXTERNC void bbFree(Bytebuffer*);
EXTERNC int bbSetalloc(Bytebuffer*,unsigned int);
EXTERNC int bbSetlength(Bytebuffer*,unsigned int);
EXTERNC int bbFill(Bytebuffer*, char fill);

/* Produce a duplicate of the contents*/
EXTERNC char* bbDup(Bytebuffer*);

/* Return the ith char; -1 if no such char */
EXTERNC int bbGet(Bytebuffer*,unsigned int);

/* Set the ith char */
EXTERNC int bbSet(Bytebuffer*,unsigned int,char);

EXTERNC int bbAppend(Bytebuffer*,char); /* Add at Tail */
EXTERNC int bbAppendn(Bytebuffer*,void*,unsigned int); /* Add at Tail */

#ifdef IGNORE
EXTERNC int bbPrepend(Bytebuffer*,char); /* Add at Head */
EXTERNC int bbPrependn(Bytebuffer*,void*,unsigned int); /* Add at Head */

/* These return and remove the specified element */
EXTERNC int bbHeadpop(Bytebuffer*,char*);
EXTERNC int bbTailpop(Bytebuffer*,char*);

/* These return but do not remove the specified element */
EXTERNC int bbHeadpeek(Bytebuffer*,char*);
EXTERNC int bbTailpeek(Bytebuffer*,char*);
#endif

EXTERNC int bbCat(Bytebuffer*,char*);
EXTERNC int bbSetcontents(Bytebuffer*, char*, unsigned int);

/* Following are always "in-lined"*/
#define bbLength(bb) ((bb)?(bb)->length:0U)
#define bbAlloc(bb) ((bb)?(bb)->alloc:0U)
#define bbContents(bb) ((bb && bb->content)?(bb)->content:(char*)"")
#define bbExtend(bb,len) bbSetalloc((bb),(len)+(bb->alloc))
#define bbClear(bb) ((bb)?(bb)->length=0:0U)
#define bbAvail(bb,n) ((bb)?((bb)->alloc - (bb)->length) > (n):0U)

#endif /*BYTEBUFFER_H*/
