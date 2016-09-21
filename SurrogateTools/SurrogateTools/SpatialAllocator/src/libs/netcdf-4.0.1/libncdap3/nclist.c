#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nclist.h"

ncelem NCDATANULL;
static int qinitialized=0;

int NClistnull(ncelem e) {return e == (ncelem)0;}

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DEFAULTALLOC 16
#define ALLOCINCR 16

NClist* nclistnew(void)
{
  NClist* sq;
  if(!qinitialized) {
    memset((void*)&NCDATANULL,0,sizeof(ncelem));
    qinitialized = 1;
  }
  sq = (NClist*)malloc(sizeof(NClist));
  if(sq) {
    sq->alloc=0;
    sq->length=0;
    sq->content=NULL;
  }
  return sq;
}

int
nclistfree(NClist* sq)
{
  if(sq) {
    sq->alloc = 0;
    if(sq->content != NULL) {free(sq->content); sq->content = NULL;}
    free(sq);
  }
  return TRUE;
}

int
nclistsetalloc(NClist* sq, unsigned int sz)
{
  ncelem* newcontent;
  if(sq == NULL) return FALSE;
  if(sz <= 0) {sz = (sq->length?2*sq->length:DEFAULTALLOC);}
  else if(sq->alloc >= sz) {return TRUE;}
  newcontent=(ncelem*)calloc(sz,sizeof(ncelem));
  if(sq->alloc > 0 && sq->length > 0 && sq->content != NULL) {
    memcpy((void*)newcontent,(void*)sq->content,sizeof(ncelem)*sq->length);
    free(sq->content);
  }
  sq->content=newcontent;
  sq->alloc=sz;
  return TRUE;
}

#ifndef LINLINE
int
nclistsetlength(NClist* sq, unsigned int sz)
{
  if(sq == NULL) return FALSE;
  if(!nclistsetalloc(sq,sz)) return FALSE;
  sq->length = sz;
  return TRUE;
}

ncelem
nclistget(NClist* sq, unsigned int index)
{
  if(sq == NULL || sq->length == 0) return NCDATANULL;
  if(index >= sq->length) return NCDATANULL;
  return sq->content[index];
}
#endif

int
nclistset(NClist* sq, unsigned int index, ncelem elem)
{
  if(sq == NULL) return FALSE;
  if(index >= sq->length) return FALSE;
  sq->content[index] = elem;
  return TRUE;
}

/* Insert at position i of sq; will push up elements i..|seq|. */
int
nclistinsert(NClist* sq, unsigned int index, ncelem elem)
{
  unsigned int i;
  if(sq == NULL) return FALSE;
  if(index > sq->length) return FALSE;
  nclistsetalloc(sq,0);
  for(i=sq->length;i>index;i--) sq->content[i] = sq->content[i-1];
  sq->content[index] = elem;
  sq->length++;
  return TRUE;
}

#ifndef LINLINE
int
nclistpush(NClist* sq, ncelem elem)
{
  if(sq == NULL) return FALSE;
  if(sq->length >= sq->alloc) nclistsetalloc(sq,0);
  sq->content[sq->length] = elem;
  sq->length++;
  return TRUE;
}
#endif

int
nclistfpush(NClist* sq, ncelem elem)
{
  unsigned int i;
  if(sq == NULL) return FALSE;
  if(sq->length >= sq->alloc) nclistsetalloc(sq,0);
  /* could we trust bcopy? instead */
  for(i=sq->alloc;i>=1;i--) {sq->content[i]=sq->content[i-1];}
  sq->content[0] = elem;
  sq->length++;
  return TRUE;
}

ncelem
nclistpop(NClist* sq)
{
  if(sq == NULL || sq->length == 0) return NCDATANULL;
  sq->length--;  
  return sq->content[sq->length];
}

ncelem
nclisttop(NClist* sq)
{
  if(sq == NULL || sq->length == 0) return NCDATANULL;
  return sq->content[sq->length - 1];
}

ncelem
nclistfpop(NClist* sq)
{
  ncelem elem;
  if(sq == NULL || sq->length == 0) return NCDATANULL;
  elem = sq->content[0];
  memcpy((void*)&sq->content[0],(void*)&sq->content[1],
           sizeof(ncelem)*(sq->length - 1));
  sq->length--;  
  return elem;
}

ncelem
nclistfront(NClist* sq)
{
  if(sq == NULL || sq->length == 0) return NCDATANULL;
  return sq->content[0];
}

ncelem
nclistremove(NClist* sq, unsigned int i)
{
  unsigned int len;
  ncelem elem;
  if(sq == NULL || (len=sq->length) == 0) return NCDATANULL;
  if(i >= len) return NCDATANULL;
  elem = sq->content[i];
  for(i++;i<len;i++) sq->content[i-1] = sq->content[i];
  sq->length--;
  return elem;  
}

/* Duplicate and return the content (null terminate) */
ncelem*
nclistdup(NClist* sq)
{
    ncelem* result = (ncelem*)malloc(sq->length+1);
    memcpy((void*)result,(void*)sq->content,sizeof(ncelem)*sq->length);
    result[sq->length] = (ncelem)0;
    return result;
}

int
nclistcontains(NClist* list, ncelem elem)
{
    unsigned int i;
    for(i=0;i<nclistlength(list);i++) {
	if(elem == nclistget(list,i)) return 1;
    }
    return 0;
}
