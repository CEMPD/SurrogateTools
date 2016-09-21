/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ocinternal.h"
#include "ocdebug.h"
#include "http.h"
#include "read.h"

extern int oc_curl_file_supported;

/*Forward*/
static int readfile(char* path, char* suffix, OCbytes* packet);
static int readfiletofile(char* path, char* suffix, FILE* stream, size_t*);

int
readDDS(CURL* curl, DAPURL* url, OCbytes* packet)
{
   return readschema(curl,url,packet,".dds");
}

int
readDAS(CURL* curl, DAPURL* url, OCbytes* packet)
{
   return readschema(curl,url,packet,".das");
}

int
readversion(CURL* curl, DAPURL* url, OCbytes* packet)
{
   return readschema(curl,url,packet,".ver");
}

int
readschema(CURL* curl, DAPURL* url, OCbytes* packet, char* suffix)
{
   int stat;

   if(strncmp(url->url,"file:",5)==0 && !oc_curl_file_supported) {
        /* Short circuit file://... urls*/
	/* We do this because the test code always needs to read files*/
	stat = readfile(url->url,suffix,packet);
    } else {
        char* fetchurl = (char*)ocmalloc(strlen(url->url)+strlen(suffix)+1);
	MEMCHECK(fetchurl,OC_ENOMEM);
	strcpy(fetchurl,url->url);
	strcat(fetchurl,suffix);
	if(ocdebug > 0) {
            fprintf(stderr,"fetch url=%s\n",fetchurl); fflush(stderr);
	}
        stat = fetch_url(curl,fetchurl,packet);
        free(fetchurl);
    }
    return THROW(stat);
}

int
readDATADDS(CURL* curl, DAPURL* url, FILE* stream, size_t* sizep)
{
    int stat;

    if(strncmp(url->url,"file:",5)==0 && !oc_curl_file_supported) {
	stat = readfiletofile(url->url, ".dods", stream, sizep);
    } else {
	char* fetchurl = (char*) ocmalloc(strlen(url->url) + strlen(".dods") + 1);
        MEMCHECK(fetchurl,OC_ENOMEM);
	strcpy(fetchurl, url->url);
	strcat(fetchurl, ".dods");
	if (ocdebug > 0) {
		fprintf(stderr, "fetch url=%s\n", fetchurl);
		fflush(stderr);
	}
	stat = file_fetch_url(curl, fetchurl, stream, sizep);
	free(fetchurl);
    }
    return THROW(stat);
}

static int
readfiletofile(char* path, char* suffix, FILE* stream, size_t* sizep)
{
    int stat;
    OCbytes* packet = ocbnew();
    size_t len;
    /* check for leading file:/// */
    if(strncmp(path,"file:///",8)==0) path += 7; /* assume absolute path*/
    stat = readfile(path,suffix,packet);
    if(stat != OC_NOERR) goto unwind;
    len = oclistlength(packet);
    if(stat == OC_NOERR) {
	size_t written;
        fseek(stream,0,SEEK_SET);
	written = fwrite(ocbcontents(packet),1,len,stream);
	if(written != len) stat = OC_EIO;
    }
    if(sizep != NULL) *sizep = len;
unwind:
    ocbfree(packet);
    return THROW(stat);
}

static int
readfile(char* path, char* suffix, OCbytes* packet)
{
    int stat;
    char buf[1024];
    char filename[1024];
    int count,size,fd;
    /* check for leading file:/// */
    if(strncmp(path,"file://",7)==0) path += 7; /* assume absolute path*/
    strcpy(filename,path);
    if(suffix != NULL) strcat(filename,suffix);
    fd = open(filename,O_RDONLY);
    if(fd < 0) {
	oclog(LOGERR,"open failed:%s",filename);
	return THROW(OC_EOPEN);
    }
    size=0;
    stat = OC_NOERR;
    for(;;) {
	count = read(fd,buf,sizeof(buf));
	if(count <= 0)
	    break;
	else if(count <  0) {
	    stat = OC_EIO;
	    oclog(LOGERR,"read failed: %s",filename);
	    break;
	}
	ocbappendn(packet,buf,count);
	size += count;
    }
    close(fd);
    return THROW(stat);
}
