/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ncbytebuffer.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DEFAULTALLOC 1024
#define ALLOCINCR 1024

int ncbdebug = 1;

/* For debugging purposes */
static long
ocbFail(void)
{
    fflush(stdout);
    fprintf(stderr,"bytebuffer failure\n");
    fflush(stderr);
    if(ncbdebug) exit(1);
    return FALSE;
}

NCbytebuffer*
ncbytesnew(void)
{
  NCbytebuffer* bb = (NCbytebuffer*)malloc(sizeof(NCbytebuffer));
  if(bb == NULL) return (NCbytebuffer*)ocbFail();
  bb->alloc=0;
  bb->length=0;
  bb->content=NULL;
  bb->nonextendible = 0;
  return bb;
}

int
ncbytessetalloc(NCbytebuffer* bb, unsigned int sz)
{
  char* newcontent;
  if(bb == NULL) return ocbFail();
  if(sz <= 0) {sz = (bb->alloc?2*bb->alloc:DEFAULTALLOC);}
  else if(bb->alloc >= sz) return TRUE;
  else if(bb->nonextendible) return ocbFail();
  newcontent=(char*)calloc(sz,sizeof(char));
  if(bb->alloc > 0 && bb->length > 0 && bb->content != NULL) {
    memcpy((void*)newcontent,(void*)bb->content,sizeof(char)*bb->length);
  }
  if(bb->content != NULL) free(bb->content);
  bb->content=newcontent;
  bb->alloc=sz;
  return TRUE;
}

void
ncbytesfree(NCbytebuffer* bb)
{
  if(bb == NULL) return;
  if(bb->content != NULL) free(bb->content);
  free(bb);
}

int
ncbytessetlength(NCbytebuffer* bb, unsigned int sz)
{
  if(bb == NULL) return ocbFail();
  if(!ncbytessetalloc(bb,sz)) return ocbFail();
  bb->length = sz;
  return TRUE;
}

int
ncbytesfill(NCbytebuffer* bb, char fill)
{
  unsigned int i;
  if(bb == NULL) return ocbFail();
  for(i=0;i<bb->length;i++) bb->content[i] = fill;
  return TRUE;
}

int
ncbytesget(NCbytebuffer* bb, unsigned int index)
{
  if(bb == NULL) return -1;
  if(index >= bb->length) return -1;
  return bb->content[index];
}

int
ncbytesset(NCbytebuffer* bb, unsigned int index, char elem)
{
  if(bb == NULL) return ocbFail();
  if(index >= bb->length) return ocbFail();
  bb->content[index] = elem;
  return TRUE;
}

int
ncbytesappend(NCbytebuffer* bb, char elem)
{
  if(bb == NULL) return ocbFail();
  if(bb->length >= bb->alloc) if(!ncbytessetalloc(bb,0)) return ocbFail();
  bb->content[bb->length] = elem;
  bb->length++;
  return TRUE;
}

/* This assumes s is a null terminated string*/
int
ncbytescat(NCbytebuffer* bb, char* s)
{
    ncbytesappendn(bb,(void*)s,strlen(s)+1); /* include trailing null*/
    /* back up over the trailing null*/
    if(bb->length == 0) return ocbFail();
    bb->length--;
    return 1;
}

int
ncbytesappendn(NCbytebuffer* bb, void* elem, unsigned int n)
{
  if(bb == NULL || elem == NULL) return ocbFail();
  if(n == 0) {n = strlen((char*)elem);}
  while(!ncbytesavail(bb,n)) {if(!ncbytessetalloc(bb,0)) return ocbFail();}
  memcpy((void*)&bb->content[bb->length],(void*)elem,n);
  bb->length += n;
  return TRUE;
}

int
ncbytesprepend(NCbytebuffer* bb, char elem)
{
  int i;
  if(bb == NULL) return ocbFail();
  if(bb->length >= bb->alloc) if(!ncbytessetalloc(bb,0)) return ocbFail();
  /* could we trust memcpy? instead */
  for(i=bb->alloc;i>=1;i--) {bb->content[i]=bb->content[i-1];}
  bb->content[0] = elem;
  bb->length++;
  return TRUE;
}

int
ncbytesprependn(NCbytebuffer* bb, char* elem, unsigned int n)
{
  unsigned int i;
  if(bb == NULL) return ocbFail();
  while(!ncbytesavail(bb,n)) {if(!ncbytessetalloc(bb,0)) return ocbFail();}
  memcpy((void*)&bb->content[bb->length],(void*)elem,n);
  /* could we trust memcpy? instead */
  for(i=bb->length+n;i>=1+n;i--) {bb->content[i]=bb->content[i-n];}
  bb->length += n;
  return TRUE;
}

int
ncbytesheadpop(NCbytebuffer* bb, char* pelem)
{
  if(bb == NULL) return ocbFail();
  if(bb->length == 0) return ocbFail();
  *pelem = bb->content[0];
  memcpy((void*)&bb->content[0],(void*)&bb->content[1],
        sizeof(char)*(bb->length - 1));
  bb->length--;
  return TRUE;
}

int
ncbytestailpop(NCbytebuffer* bb, char* pelem)
{
  if(bb == NULL) return ocbFail();
  if(bb->length == 0) return ocbFail();
  *pelem = bb->content[bb->length-1];
  bb->length--;
  return TRUE;
}

int
ncbytesheadpeek(NCbytebuffer* bb, char* pelem)
{
  if(bb == NULL) return ocbFail();
  if(bb->length == 0) return ocbFail();
  *pelem = bb->content[0];
  return TRUE;
}

int
ncbytestailpeek(NCbytebuffer* bb, char* pelem)
{
  if(bb == NULL) return ocbFail();
  if(bb->length == 0) return ocbFail();
  *pelem = bb->content[bb->length - 1];
  return TRUE;
}

char*
ncbytesdup(NCbytebuffer* bb)
{
    char* result = (char*)malloc(bb->length+1);
    memcpy((void*)result,(const void*)bb->content,bb->length);
    result[bb->length] = '\0'; /* just in case it is a string*/
    return result;
}

int
ncbytessetcontents(NCbytebuffer* bb, char* contents, unsigned int alloc)
{
    if(bb == NULL) return ocbFail();
    ncbytesclear(bb);
    if(!bb->nonextendible && bb->content != NULL) free(bb->content);
    bb->content = contents;
    bb->length = 0;
    bb->alloc = alloc;
    bb->nonextendible = 1;
    return 1;
}
