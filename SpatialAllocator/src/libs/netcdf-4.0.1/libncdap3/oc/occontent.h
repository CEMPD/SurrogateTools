/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCCONTENT_H
#define OCCONTENT_H

/*! Specifies the OCcontent*/
typedef struct OCcontent {
    OCmode mode;
    struct OCconnection* state;
    struct OCnode* node;
    unsigned int xdroffset;
    size_t index; /* dim, record, or field*/
    int    packed; /* track OC_String and OC_Byte specially*/
    struct OCcontent* next;
    OCmemdata* memdata; /* iff compiled */
    void* public; /* temporary*/
} OCcontent;

extern struct OCcontent* oc_newcontent(struct OCconnection*);
extern void oc_freecontent(struct OCconnection*, struct OCcontent*);

extern int oc_rootcontent(struct OCconnection*, struct OCcontent*);
extern int oc_dimcontent(struct OCconnection*, struct OCcontent*, struct OCcontent*, size_t);
extern int oc_recordcontent(struct OCconnection*, struct OCcontent*, struct OCcontent*, size_t);
extern int oc_fieldcontent(struct OCconnection*, struct OCcontent*, struct OCcontent*, size_t);

extern int oc_getcontent(OCconnection*, struct OCcontent*, void* memory, size_t memsize,
                            size_t start, size_t count);


extern size_t oc_dimcount(struct OCconnection*, struct OCcontent*);
extern size_t oc_recordcount(struct OCconnection*, struct OCcontent*);
extern size_t oc_fieldcount(struct OCconnection*, struct OCcontent*);

extern OCmode oc_contentmode(struct OCconnection* conn, struct OCcontent*);
extern struct OCnode* oc_contentnode(struct OCconnection* conn, struct OCcontent*);
extern size_t oc_contentindex(struct OCconnection* conn, struct OCcontent*);

/* These are not really for external use.*/
extern struct OCcontent* oc_resetcontent(struct OCconnection*, OCcontent*);
extern struct OCcontent* oc_clonecontent(struct OCconnection*, OCcontent*);

#endif /*OCCONTENT_H*/
