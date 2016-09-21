/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCLOG_H
#define OCLOG_H

/* Tag kind of log entry*/
#define LOGNOTE 0
#define LOGWARN 1
#define LOGERR 2
#define LOGDBG 3

extern void ocloginit(void);
extern void ocsetlogging(int tf);
extern void ocopenlog(char* file);
extern void occloselog(void);

extern void oclog(int tag, const char* fmt, ...);
extern void oclogtext(const char* text);

#endif /*OCLOG_H*/
