/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef READ_H
#define READ_H


extern int readDDS(CURL*, DAPURL*, OCbytes*);
extern int readDAS(CURL*, DAPURL*, OCbytes*);
extern int readversion(CURL*, DAPURL*, OCbytes*);
extern int readschema(CURL*, DAPURL*, OCbytes*, char*);
extern int readDATADDS(CURL*, DAPURL*, FILE*, size_t*);

#endif /*READ_H*/
