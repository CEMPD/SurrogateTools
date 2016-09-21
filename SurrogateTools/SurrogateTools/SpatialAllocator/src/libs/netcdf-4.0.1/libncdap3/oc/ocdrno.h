/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCDRNO_H
#define OCDRNO_H
/*
This file exports procedures
that access the internals of
oc. They are intended to be called
by the drno code to avoid at least
the appearance of breaking the oc
encapsulation. 
*/

extern int oc_stringcontent(OCconnection*, OCcontent*, char**, size_t*);

#endif /*OCDRNO_H*/
