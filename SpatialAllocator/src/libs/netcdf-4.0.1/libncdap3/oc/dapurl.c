/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include "ocdebug.h"

#define LBRACKET '['
#define RBRACKET ']'


/* Forward reference */
static int decodeparameters(DAPURL*,char*);

static char* legalprotocols[] = {
"file:",
"http:",
"https:",
"ftp:",
NULL /* NULL terminate*/
};


/* Do a simple url parse*/
int
dapurlparse(const char* url0, DAPURL* dapurl)
{
    char* url;
    char** pp;
    char* p;
    char* p1;
    int c;
    /* accumulate parse points*/
    char* params = NULL;
    char* baseurl = NULL;
    char* proj = NULL;
    char* select = NULL;
    char* stop;
    size_t baselen, projlen, sellen;

    memset((void*)dapurl,0,sizeof(DAPURL));

    /* copy url and remove all whitespace*/
    url = strdup(url0);

    p = url;
    p1 = url;
    while((c=*p1++)) {if(c != ' ' && c != '\t') *p++ = c;}

    p = url;
    stop = p + strlen(p);

    /* break up the url string into pieces*/
    if(*p == LBRACKET) {
	params = p+1; /* leave off the leading lbracket*/
	/* find end of the clientparams*/
        for(;*p;p++) {if(p[0] == RBRACKET && p[1] != LBRACKET) break;}
	if(*p == 0) goto fail; /* malformed client params*/
	*p = '\0'; /* leave off the trailing rbracket	*/
	p++; /* move past the params*/
    }

    /* verify that the url starts with an acceptable protocol*/
    for(pp=legalprotocols;*pp;pp++) {
        if(strncmp(p,*pp,strlen(*pp))==0) break;
    }
    if(*pp == NULL) goto fail; /* illegal protocol*/

    baseurl = p;

    /* look for '?' or '&'*/
    proj = strchr(p,'?');
    if(proj != NULL) {
	select = strchr(proj,'&');
    } else {
	select = strchr(p,'&');
    }
    baselen = 0; projlen = 0; sellen = 0;
    if(proj != NULL) {
        baselen = (proj - baseurl);
    } else if(select != NULL) {
        baselen = (select - baseurl);
    } else {
	baselen = (stop - baseurl);
    }
    if(proj != NULL && select == NULL) projlen = (stop - proj);
    if(proj != NULL && select != NULL) projlen = (select - proj);
    if(select != NULL) sellen = (stop - select);

    if(projlen == 1 || sellen == 1) return 0; /* invalid syntax*/

    /* assemble the component pieces*/
    dapurl->url = ocstrndup(baseurl,baselen);
    /* Leave off the leading ? for project*/
    if(proj != NULL) dapurl->projection = ocstrndup(proj+1,projlen-1);
    if(select != NULL) dapurl->selection = ocstrndup(select,sellen);
    if(ocdebug) {
        fprintf(stderr,"dapurl: url=|%s| projection=|%s| selection=|%s|\n",
		dapurl->url, dapurl->projection, dapurl->selection);

    }
    /* parse the parameters, if any*/
    if(params != NULL) {
	if(!decodeparameters(dapurl,params)) goto fail;
    }
    free(url);
    return 1;
fail:
    if(url != NULL) free(url);
    return 0;
}

static int
decodeparameters(DAPURL* dapurl, char* params0)
{
    char* cp;
    char* cq;
    char** ep;
    int c;
    int i;
    int nparams;
    char** envv = NULL;
    char* params = strdup(params0);
    char* params1 = strdup(params);

    /* Pass 1 to replace "][" pairs with ','*/
    cp=params; cq = params1;
    while((c=*cp++)) {
	if(c == RBRACKET && *cp == LBRACKET) {cp++; c = ',';}
	*cq++ = c;
    }
    *cq = '\0';
    free(params);
    params = params1;

    /* Pass 2 to count # of params and break string*/
    nparams = 0;
    for(cp=params;(c=*cp);cp++) {
	if(c == ',') {*cp = '\0'; nparams++;}
    }
    nparams++; /* Add one for the last parameter*/
    envv = (char**)ocmalloc(sizeof(char*)*((2*nparams)+1));
    MEMCHECK(envv,0);

    /* Pass 3 to break up each pass into a (name,value) pair*/
    /* and insert into the envv*/
    /* parameters of the form name name= are converted to name=""*/
    cp = params;
    ep = envv;
    for(i=0;i<nparams;i++) {
	char* next = cp+strlen(cp)+1; /* save ptr to next pair*/
	char* vp;
	/*break up the ith param*/
	vp = strchr(cp,'=');
	if(vp != NULL) {*vp = '\0'; vp++;} else {vp = "";}
        *ep++ = strdup(cp);
	*ep++ = strdup(vp);
	cp = next;
    }
    *ep = NULL; /* null terminate*/
    free(params);
    dapurl->params = envv;
    return 1;
}

/* Call must free the actual url instance.*/
void
dapurlclear(DAPURL* dapurl)
{
    if(dapurl->url != NULL) {free(dapurl->url); dapurl->url = NULL;}
    if(dapurl->projection != NULL) {free(dapurl->projection); dapurl->projection = NULL;}
    if(dapurl->selection != NULL) {free(dapurl->selection); dapurl->selection = NULL;}
    if(dapurl->params != NULL) {
	char** pp;
	for(pp=dapurl->params;*pp;pp+=2) {
	    free(pp[0]);
	    if(pp[1] != NULL) free(pp[1]);
        }
	free(dapurl->params);
	dapurl->params = NULL;
    }
}

const char*
dapurllookup(DAPURL* durl, const char* clientparam)
{
    char** params = durl->params;
    if(params == NULL || clientparam == NULL) return NULL;
    while(*params) {
	char* name = *params++;
	char* value = *params++;
	if(strcmp(clientparam,name)==0) return value;
    }
    return NULL;
}

int
dapurlreplace(DAPURL* durl, const char* clientparam, const char* newvalue)
{
    char** params = durl->params;
    unsigned int nstrings = 0;
    char** location = NULL;

    if(params == NULL || clientparam == NULL) return 0;
    /* simultaneously count and search*/
    while(*params) {
	char* name = *params;
	if(strcmp(clientparam,name)==0) {location = params;}
	nstrings++;
	params += 2;
    }
    if(location == NULL) return 0; /* not found*/
    if(newvalue != NULL) { /* replace*/
	ocfree(location[1]);
	location[1] = strdup(newvalue);
    } else { /* delete*/
	unsigned int count, rem;
	count = (nstrings*2)+1; /* number of char* instances in durl->params*/
	rem = ((durl->params+count) - (location+2));
	ocfree(location[0]);
	ocfree(location[1]);
	memcpy((void*)location,(void*)(location+2),rem*sizeof(char*));
    }
    return 1;
}
