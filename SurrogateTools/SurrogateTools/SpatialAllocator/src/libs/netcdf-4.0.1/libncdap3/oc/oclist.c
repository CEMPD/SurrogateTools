#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oclist.h"

ocelem OCDATANULL;
static int qinitialized=0;

int oclistnull(ocelem e) {return e == (ocelem)0;}

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DEFAULTALLOC 16
#define ALLOCINCR 16

OClist* oclistnew(void)
{
  OClist* sq;
  if(!qinitialized) {
    memset((void*)&OCDATANULL,0,sizeof(ocelem));
    qinitialized = 1;
  }
  sq = (OClist*)malloc(sizeof(OClist));
  if(sq) {
    sq->alloc=0;
    sq->length=0;
    sq->content=NULL;
  }
  return sq;
}

int
oclistfree(OClist* sq)
{
  if(sq) {
    sq->alloc = 0;
    if(sq->content != NULL) {free(sq->content); sq->content = NULL;}
    free(sq);
  }
  return TRUE;
}

int
oclistsetalloc(OClist* sq, unsigned int sz)
{
  ocelem* newcontent;
  if(sq == NULL) return FALSE;
  if(sz <= 0) {sz = (sq->length?2*sq->length:DEFAULTALLOC);}
  else if(sq->alloc >= sz) {return TRUE;}
  newcontent=(ocelem*)calloc(sz,sizeof(ocelem));
  if(sq->alloc > 0 && sq->length > 0 && sq->content != NULL) {
    memcpy((void*)newcontent,(void*)sq->content,sizeof(ocelem)*sq->length);
    free(sq->content);
  }
  sq->content=newcontent;
  sq->alloc=sz;
  return TRUE;
}

#ifndef LINLINE
int
oclistsetlength(OClist* sq, unsigned int sz)
{
  if(sq == NULL) return FALSE;
  if(!oclistsetalloc(sq,sz)) return FALSE;
  sq->length = sz;
  return TRUE;
}

ocelem
oclistget(OClist* sq, unsigned int index)
{
  if(sq == NULL || sq->length == 0) return OCDATANULL;
  if(index >= sq->length) return OCDATANULL;
  return sq->content[index];
}
#endif

int
oclistset(OClist* sq, unsigned int index, ocelem elem)
{
  if(sq == NULL) return FALSE;
  if(index >= sq->length) return FALSE;
  sq->content[index] = elem;
  return TRUE;
}

/* Insert at position i of sq; will push up elements i..|seq|. */
int
oclistinsert(OClist* sq, unsigned int index, ocelem elem)
{
  unsigned int i;
  if(sq == NULL) return FALSE;
  if(index > sq->length) return FALSE;
  oclistsetalloc(sq,0);
  for(i=sq->length;i>index;i--) sq->content[i] = sq->content[i-1];
  sq->content[index] = elem;
  sq->length++;
  return TRUE;
}

#ifndef LINLINE
int
oclistpush(OClist* sq, ocelem elem)
{
  if(sq == NULL) return FALSE;
  if(sq->length >= sq->alloc) oclistsetalloc(sq,0);
  sq->content[sq->length] = elem;
  sq->length++;
  return TRUE;
}
#endif

int
oclistfpush(OClist* sq, ocelem elem)
{
  unsigned int i;
  if(sq == NULL) return FALSE;
  if(sq->length >= sq->alloc) oclistsetalloc(sq,0);
  /* could we trust bcopy? instead */
  for(i=sq->alloc;i>=1;i--) {sq->content[i]=sq->content[i-1];}
  sq->content[0] = elem;
  sq->length++;
  return TRUE;
}

ocelem
oclistpop(OClist* sq)
{
  if(sq == NULL || sq->length == 0) return OCDATANULL;
  sq->length--;  
  return sq->content[sq->length];
}

ocelem
oclisttop(OClist* sq)
{
  if(sq == NULL || sq->length == 0) return OCDATANULL;
  return sq->content[sq->length - 1];
}

ocelem
oclistfpop(OClist* sq)
{
  ocelem elem;
  if(sq == NULL || sq->length == 0) return OCDATANULL;
  elem = sq->content[0];
  memcpy((void*)&sq->content[0],(void*)&sq->content[1],
           sizeof(ocelem)*(sq->length - 1));
  sq->length--;  
  return elem;
}

ocelem
oclistfront(OClist* sq)
{
  if(sq == NULL || sq->length == 0) return OCDATANULL;
  return sq->content[0];
}

ocelem
oclistremove(OClist* sq, unsigned int i)
{
  unsigned int len;
  ocelem elem;
  if(sq == NULL || (len=sq->length) == 0) return OCDATANULL;
  if(i >= len) return OCDATANULL;
  elem = sq->content[i];
  for(i++;i<len;i++) sq->content[i-1] = sq->content[i];
  sq->length--;
  return elem;  
}

/* Duplicate and return the content (null terminate) */
ocelem*
oclistdup(OClist* sq)
{
    ocelem* result = (ocelem*)malloc(sq->length+1);
    memcpy((void*)result,(void*)sq->content,sizeof(ocelem)*sq->length);
    result[sq->length] = (ocelem)0;
    return result;
}

