/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef HTTP_H
#define HTTP_H 1

extern int curlopen(CURL** curlp);
extern void curlclose(CURL*);

extern int fetch_url(CURL*, char*, OCbytes*);
extern int file_fetch_url(CURL*, char*, FILE*, size_t*);

#endif /*HTTP_H*/
