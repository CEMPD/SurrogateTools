#ifndef NCLIST_H
#define NCLIST_H 1

/* Define the type of the elements in the sequence*/

#if defined(_CPLUSPLUS_) || defined(__CPLUSPLUS__)
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

typedef unsigned long ncelem;

EXTERNC int NClistnull(ncelem);

typedef struct NClist {
  unsigned int alloc;
  unsigned int length;
  ncelem* content;
} NClist;

EXTERNC NClist* nclistnew(void);
EXTERNC int nclistfree(NClist*);
EXTERNC int nclistsetalloc(NClist*,unsigned int);

/* Set the ith element of sq */
EXTERNC int nclistset(NClist*,unsigned int,ncelem);
/* Insert at position i of sq; will push up elements i..|seq|. */
EXTERNC int nclistinsert(NClist*,unsigned int,ncelem);

/* Tail operations */
EXTERNC int nclistpush(NClist*,ncelem); /* Add at Tail */
EXTERNC ncelem nclistpop(NClist*);
EXTERNC ncelem nclisttop(NClist*);

/* Head operations */
EXTERNC int nclistfpush(NClist*,ncelem); /* Add at Head */
EXTERNC ncelem nclistfpop(NClist*);
EXTERNC ncelem nclistfront(NClist*);
EXTERNC ncelem nclistremove(NClist* sq, unsigned int i);

/* Duplicate and return the content (null terminate) */
EXTERNC ncelem* nclistdup(NClist*);

/* Look for value match */
EXTERNC int nclistcontains(NClist*, ncelem);

/* Following are always "in-lined"*/
#define nclistclear(sq) nclistsetlength((sq),0U)
#define nclistextend(sq,len) nclistsetalloc((sq),(len)+(sq->alloc))
#define nclistcontents(sq) ((sq)->content)
#define nclistlength(sq)  ((sq)?(sq)->length:0U)

/* Following can be open-coded via macros */
#ifdef LINLINE

EXTERNC ncelem NCDATANULL;

#define nclistsetlength(sq,sz) \
(((sq)==NULL||(sz)<0||!nclistsetalloc((sq),(sz)))?0:((sq)->length=(sz),1))

#define nclistget(sq,index) \
(((sq)==NULL||(sz)<0||(index)<0||(index)>=(sq)->length)?NCDATANULL:((sq)->content[index]))

#define nclistpush(sq,elem) \
(((sq)==NULL||(((sz)->length >= (sq)->alloc)&&!nclistsetalloc((sq),0)))?0:((sq)->content[(sq)->length++]=(elem),1))

#else
EXTERNC int nclistsetlength(NClist*,unsigned int);
EXTERNC ncelem nclistget(NClist*,unsigned int);/* Return the ith element of sq */
EXTERNC int nclistpush(NClist*,ncelem); /* Add at Tail */
#endif


#endif /*NCLIST_H*/

