/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H 1

/*! Specifies an OCslice. */
typedef struct OCslice {
    size_t first;
    size_t count;
    size_t stride;
    size_t stop; /* == first + count */
} OCslice;

/*! Specifies a form of path where each element can have a set of associated indices */
typedef struct Path {
    OClist* names;
    OClist* indexsets; /* oclist<oclist<Slice>> */
} Path;

/*! Specifies a ProjectionClause. */
typedef struct Projectionclause {
    char* target; /* "variable name" as mentioned in the projection */
    OClist* indexsets; /* oclist<oclist<OCslice>> */
    struct OCnode* node; /* node with name matching target, if any. */
    int    gridconstraint; /*  used only for testing purposes */
} Projectionclause;

/*! Selection is the node type for selection expression trees */
typedef struct Selectionclause {
    int op;
    char* value;
    struct Selectionclause* lhs;
    OClist* rhs;
} Selectionclause;


#endif /*CONSTRAINTS_H*/
