/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include <stdio.h>
#include <fcntl.h>

#define PREFIXLEN 8

#define ENVFLAG "OCLOGFILE"

static int logging;
static char* logfile;
static FILE* logstream;

void
ocloginit(void)
{
    logging = 0;
    logfile = NULL;
    logstream = NULL;
    /* Use environment variables to preset logging state*/
    /* I hope this is portable*/
    if(getenv(ENVFLAG) != NULL) {
	char* file = getenv(ENVFLAG);
	logging = 1;
	ocopenlog(file);
    }
}

void
ocsetlogging(int tf)
{
    logging = tf;
}

void
ocopenlog(char* file)
{
    if(logfile != NULL) {
	fclose(logstream);
	free(logfile);
	logfile = NULL;
    }
    if(file == NULL || strlen(file) == 0) {
	/* use stderr*/
	logstream = stderr;
	logfile = NULL;
    } else {
	int fd;
	logfile = strdup(file);
	logstream = NULL;
	/* We need to deal with this file carefully
	   to avoid unauthorized access*/
	fd = open(logfile,O_WRONLY|O_APPEND|O_CREAT,0600);
	if(fd >= 0) {
	    logstream = fdopen(fd,"a");
	} else {
	    free(logfile);
	    logfile = NULL;
	    logging = 0;
	}
    }
}

void
occloselog(void)
{
    if(logfile != NULL && logstream != NULL) {
	fclose(logstream);
	logstream = NULL;
	logfile = NULL;
    }
}

void
oclog(int tag, const char* fmt, ...)
{
    va_list args;
    char* prefix;
    if(!logging || logstream == NULL) return;

    switch (tag) {
    case LOGWARN: prefix = "Warning:"; break;
    case LOGERR:  prefix = "Error:  "; break;
    case LOGNOTE: prefix = "Note:   "; break;
    case LOGDBG:  prefix = "Debug:  "; break;
    default:
        fprintf(logstream,"Error:  Bad log prefix: %d\n",tag);
	prefix = "Error:  ";
	break;
    }
    fprintf(logstream,"%s:",prefix);

    if(fmt != NULL) {
      va_start(args, fmt);
      vfprintf(logstream, fmt, args);
      va_end( args );
    }
    fprintf(logstream, "\n" );
    fflush(logstream);
}

void
oclogtext(const char* text)
{
    char line[1024];
    size_t delta = 0;
    const char* eol = text;

    if(!logging || logstream == NULL) return;

    while(*text) {
	eol = strchr(text,'\n');
	if(eol == NULL)
	    delta = strlen(text);
	else
	    delta = (eol - text);
	if(delta > 0) memcpy(line,text,delta);
	line[delta] = '\0';
	fprintf(logstream,"        %s\n",line);
	text = eol+1;
    }
}
