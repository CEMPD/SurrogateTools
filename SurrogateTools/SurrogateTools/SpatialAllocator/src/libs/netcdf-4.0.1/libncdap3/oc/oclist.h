#ifndef OCLIST_H
#define OCLIST_H 1

/* Define the type of the elements in the sequence*/

#if defined(_CPLUSPLUS_) || defined(__CPLUSPLUS__)
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

typedef unsigned long ocelem;

EXTERNC int oclistnull(ocelem);

typedef struct OClist {
  unsigned int alloc;
  unsigned int length;
  ocelem* content;
} OClist;

EXTERNC OClist* oclistnew(void);
EXTERNC int oclistfree(OClist*);
EXTERNC int oclistsetalloc(OClist*,unsigned int);

/* Set the ith element of sq */
EXTERNC int oclistset(OClist*,unsigned int,ocelem);
/* Insert at position i of sq; will push up elements i..|seq|. */
EXTERNC int oclistinsert(OClist*,unsigned int,ocelem);

/* Tail operations */
EXTERNC int oclistpush(OClist*,ocelem); /* Add at Tail */
EXTERNC ocelem oclistpop(OClist*);
EXTERNC ocelem oclisttop(OClist*);

/* Head operations */
EXTERNC int oclistfpush(OClist*,ocelem); /* Add at Head */
EXTERNC ocelem oclistfpop(OClist*);
EXTERNC ocelem oclistfront(OClist*);
EXTERNC ocelem oclistremove(OClist* sq, unsigned int i);

/* Duplicate and return the content (null terminate) */
EXTERNC ocelem* oclistdup(OClist*);

/* Following are always "in-lined"*/
#define oclistclear(sq) oclistsetlength((sq),0U)
#define oclistextend(sq,len) oclistsetalloc((sq),(len)+(sq->alloc))
#define oclistcontents(sq) ((sq)->content)
#define oclistlength(sq)  ((sq)?(sq)->length:0U)

/* Following can be open-coded via macros */
#ifdef LINLINE

EXTERNC ocelem OCDATANULL;

#define oclistsetlength(sq,sz) \
(((sq)==NULL||(sz)<0||!oclistsetalloc((sq),(sz)))?0:((sq)->length=(sz),1))

#define oclistget(sq,index) \
(((sq)==NULL||(sz)<0||(index)<0||(index)>=(sq)->length)?OCDATANULL:((sq)->content[index]))

#define oclistpush(sq,elem) \
(((sq)==NULL||(((sz)->length >= (sq)->alloc)&&!oclistsetalloc((sq),0)))?0:((sq)->content[(sq)->length++]=(elem),1))

#else
EXTERNC int oclistsetlength(OClist*,unsigned int);
EXTERNC ocelem oclistget(OClist*,unsigned int);/* Return the ith element of sq */
EXTERNC int oclistpush(OClist*,ocelem); /* Add at Tail */
#endif


#endif /*OCLIST_H*/

