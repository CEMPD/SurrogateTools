/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include <unistd.h>
#include "ocinternal.h"
#include "ocdebug.h"
#include "ocdata.h"
#include "occontent.h"
#include "rc.h"

#include "http.h"
#include "read.h"
#include <fcntl.h>
#include <errno.h>

/* Note: TMPPATH must end in '/' */
#ifdef __CYGWIN__
/*#define TMPPATH "c:/temp/"*/
#else
/*#define TMPPATH "/tmp/"*/
#endif
#define TMPPATH "./"
#define BUFSIZE 512
#define DODSRC_SIZE 9
#define DODSRC "/.dodsrc"

static int oc_extract(OCconnection* state);

extern OCnode* makeunlimiteddimension(void);

/* Global flags*/
int oc_invert_xdr_double;
int oc_curl_file_supported;

static int initialized = 0;

static int
initialize(void)
{
    int stat = OC_NOERR;
	char buf[BUFSIZE];
	char *env;
	int len;
	/* It turns out that various machines
	   store double in different formats.
	   This affects the conversion code ocdata.c
	   when reconstructing*/
	union {
		double dv;
		unsigned int iv[2];
		char tmp[sizeof(double)];
	} udub;
	XDR xdrs;
	double testdub = 18000;
	xdrmem_create(&xdrs, (caddr_t) udub.tmp, sizeof(udub.tmp), XDR_ENCODE);
	xdr_double(&xdrs, &testdub);
	udub.iv[0] = ntohl(udub.iv[0]);
	udub.iv[1] = ntohl(udub.iv[1]);
	if (udub.dv == testdub)
		oc_invert_xdr_double = 0;
	else {
		unsigned int part = udub.iv[0];
		udub.iv[0] = udub.iv[1];
		udub.iv[1] = part;
		if (udub.dv != testdub)
			ocpanic("cannot unpack xdr_double");
		oc_invert_xdr_double = 1;
	}
	ocloginit();

	/* read/write configuration file */
	env = getenv("HOME");
	if (env != NULL) {
		len = strlen(env);
		if (len >= BUFSIZE - DODSRC_SIZE) {
			oclog(LOGERR, "length of home directory is too long\n");
			stat = OC_EIO;
			goto end;
		}
		strncpy(buf, env, BUFSIZE - 1);
		buf[len] = '\0';
		strncat(buf, DODSRC, BUFSIZE - 1);
		buf[len + DODSRC_SIZE] = '\0';

		if (ocdebug > 1)
			fprintf(stderr, "Your RC file: %s\n", buf);

		/* stat = OC_NOERR; */
		if (access(buf, R_OK) != 0) {
			if (write_dodsrc(buf) != OC_NOERR) {
				oclog(LOGERR, "Error getting buffer\n");
				stat = OC_EIO;
			}
		}

		if (read_dodsrc(buf) != OC_NOERR) {
			oclog(LOGERR, "Error parsing buffer\n");
			stat = OC_EIO;
		}
	}

	/* Determine if this version of curl supports "file://..." urls.*/
	{
            const char* const* proto; /*weird*/
	    curl_version_info_data* curldata;
	    curldata = curl_version_info(CURLVERSION_NOW);
	    oc_curl_file_supported = 0;
	    for(proto=curldata->protocols;*proto;proto++) {
		if(strcmp("file",*proto)==0) {oc_curl_file_supported=1;break;}
	    }
	    if(ocdebug > 0) {
		oclog(LOGNOTE,"Curl file:// support = %d",oc_curl_file_supported);
	    }
	}

	initialized = 1;

end:
	return THROW(stat);
}

/**************************************************/
int
oc_open(OCconnection** statep)
{
    int stat = OC_NOERR;
    int fd;
    OCconnection * state = NULL;

    if(!initialized) {
	stat = initialize();
	if(stat != OC_NOERR) return THROW(stat);
    }

    state = (OCconnection*)ocmalloc(sizeof(OCconnection)); /* ocmalloc zeros memory*/
    if(state == NULL) {stat = OC_ENOMEM; goto unwind;}

    /* Setup DAP state*/
    stat = curlopen(&state->dap.curl);
    if(stat != OC_NOERR) return THROW(stat);
    state->dap.packet = ocbnew();

    /* Create the datadds file immediately
       so that DRNO can reference it*/

    /* Make the tmp file*/
    {
        int c,slen = strlen(TMPPATH) + strlen("datadds") + strlen("XXXXXX");
        char* name = (char*)ocmalloc(slen+1);
	char* p;
        MEMCHECK(name,OC_ENOMEM);
	strcpy(name,TMPPATH);
        strcpy(name,"datadds");
        strcat(name,"XXXXXX");
        p = name + strlen("datadds");
        /* \', and '/' to '_' and '.' to '-'*/
        for(;(c=*p);p++) {
	    if(c == '\\' || c == '/') {*p = '_';}
	    else if(c == '.') {*p = '-';}
	}
	/* Note Potential problem: old versions of this function
           leave the file in mode 0666 instead of 0600 */
        fd = mkstemp(name);
        if(fd < 0) {
	    oclog(LOGERR,"oc_open: attempt to open tmp file %s failed",name);
  	    stat = errno;
	    goto unwind;
        }
	state->dap.filename = name; /* remember our tmp file name */
	state->dap.file = fdopen(fd,"w+");
        if(state->dap.file == NULL) {stat = OC_EOPEN; goto unwind;}
    }

    *statep = state;
    return THROW(stat);
unwind:
    if(state->dap.file != NULL) fclose(state->dap.file);
    if(state->dap.filename != NULL) {
	unlink(state->dap.filename);
	free(state->dap.filename);
    }
    if(state != NULL) ocfree(state);
    return THROW(stat);
}

OCnode*
oc_getdds(struct OCconnection* state)
{
    return state->schema.dds.tree;
}

OCnode*
oc_getdas(struct OCconnection* state)
{
    return state->schema.das.tree;
}

OCnode*
oc_getdatadds(struct OCconnection* state)
{
    return state->schema.datadds.tree;
}

OClist*
oc_getddsall(struct OCconnection* state)
{
    return state->schema.dds.nodes;
}

OClist*
oc_getdasall(struct OCconnection* state)
{
    return state->schema.das.nodes;
}

OClist*
oc_getdataddsall(struct OCconnection* state)
{
    return state->schema.datadds.nodes;
}

const char*
oc_getdastext(struct OCconnection* state)
{
    return state->schema.das.text;
}

const char*
oc_getddstext(struct OCconnection* state)
{
    return state->schema.dds.text;
}

const char*
oc_getdataddstext(struct OCconnection* state)
{
    return state->schema.datadds.text;
}

void
oc_dataddsclear(struct OCconnection* state)
{
    if(state->schema.datadds.tree != NULL) {
	ocfreenodes(state->schema.datadds.nodes);
        oclistfree(state->schema.datadds.nodes);
    }
    ocfree((void*)state->schema.datadds.text);
    dapurlclear(&state->schema.datadds.url);
    if(state->dap.xdrs != NULL) {
	xdr_destroy(state->dap.xdrs);
	ocfree(state->dap.xdrs);
	state->dap.xdrs = NULL;
    }
    state->schema.datadds.tree = NULL;
    state->schema.datadds.nodes = NULL;
    state->schema.datadds.text = NULL;
}

void
oc_dasclear(struct OCconnection* state)
{
    if(state->schema.das.tree != NULL) {
	ocfreenodes(state->schema.das.nodes);
        oclistfree(state->schema.das.nodes);
    }
    ocfree((void*)state->schema.das.text);
    dapurlclear(&state->schema.das.url);
    state->schema.das.tree = NULL;
    state->schema.das.nodes = NULL;
    state->schema.das.text = NULL;
}

void
oc_ddsclear(struct OCconnection* state)
{
    if(state->schema.dds.tree != NULL) {
	ocfreenodes(state->schema.dds.nodes);
        oclistfree(state->schema.dds.nodes);
    }
    ocfree((void*)state->schema.dds.text);
    dapurlclear(&state->schema.dds.url);
    state->schema.dds.tree = NULL;
    state->schema.dds.nodes = NULL;
    state->schema.dds.text = NULL;
}


int
oc_fetchdas(OCconnection* state, const char* url)
{
    int stat = OC_NOERR;
    DAPURL tmpurl;
    OCDXD* dxd;

    if(!dapurlparse(url,&tmpurl)) {
	return THROW(OC_EBADURL);
    }

    oc_dasclear(state);

    dxd = &state->schema.das;

    dxd->url = tmpurl;

    ocbclear(state->dap.packet);
    stat = readDAS(state->dap.curl,&dxd->url,state->dap.packet);
    if(stat != OC_NOERR) {
	oclog(LOGWARN,"fetchdas: Could not read DAS file; ignored");
	return THROW(stat);
    }

    dxd->text = ocbdup(state->dap.packet);
    ocbclear(state->dap.packet);

    dxd->nodes = oclistnew();
    dxd->tree = DAPparse(state,dxd->text,dxd->nodes);
    if(dxd->tree == NULL) {
	oclog(LOGERR,"oc_open:DAS parse failed\n");
	return THROW(OC_EINVAL);
    }
    if(dxd->tree->octype != OC_Attributeset) {
	oclog(LOGERR,"oc_open:URL is not a legal DAS\n");
	return THROW(OC_EINVAL);
    }

    /* Add fullname info*/
    computefullnames(dxd->tree);

    DEBUG(2,"DAS dump:");
    DEBUGTEXT(2,dxd->text);

    return THROW(stat);
}

int
oc_fetchdds(OCconnection* state, const char* url)
{
    int stat = OC_NOERR;
    DAPURL tmpurl;
    OCDXD* dxd;

    if(!dapurlparse(url,&tmpurl)) {
	return THROW(OC_EBADURL);
    }

    oc_ddsclear(state);

    dxd = &state->schema.dds;

    dxd->url = tmpurl;

    ocbclear(state->dap.packet);
    stat = readDDS(state->dap.curl,&dxd->url,state->dap.packet);
    if(stat != OC_NOERR) return THROW(stat);

    dxd->text = ocbdup(state->dap.packet);
    ocbclear(state->dap.packet);

    dxd->nodes = oclistnew();
    dxd->tree = DAPparse(state,dxd->text,dxd->nodes);
    if(dxd->tree == NULL) {
	oclog(LOGERR,"oc_open:DDS parse failed\n");
	return THROW(OC_EINVAL);
    }
    if(dxd->tree->octype != OC_Dataset) {
	oclog(LOGERR,"oc_open:URL is not a legal DDS\n");
	return THROW(OC_EINVAL);
    }
    dxd->tree->datadds = 0; /* not a data dds*/

    /* Process ocnodes to fix various semantic issues*/
    computeocsemantics(dxd->nodes);

    /* Process ocnodes to compute name info*/
    computefullnames(dxd->tree);

    DEBUG(2,"DDS dump:");
    DEBUGTEXT(2,dxd->text);

    return THROW(stat);
}

int
oc_fetchdatadds(OCconnection* state, const char* url)
{
    int stat;
    DAPURL tmpurl;
    size_t mark;
    OCDXD* dxd;

    stat = OC_NOERR;
    if(!dapurlparse(url,&tmpurl)) {
	return THROW(OC_EBADURL);
    }

    oc_dataddsclear(state);

    dxd = &state->schema.datadds;

    dxd->url = tmpurl;

    /* close the tmp, and reopen and truncate it*/
    if(state->dap.filename == NULL) return THROW(OC_EINVAL);
    if(state->dap.file != NULL) fclose(state->dap.file);

    state->dap.file = fopen(state->dap.filename,"w+"); /* W+ => rw+truncate*/
    if(state->dap.file == NULL) {stat = OC_EOPEN; goto unwind;}

    stat = readDATADDS(state->dap.curl,&dxd->url,state->dap.file,&mark);
    if(stat != OC_NOERR) goto unwind;
    state->dap.filesize = mark;

    /* Reset the file seek position to the beginning*/
    fseek(state->dap.file,0,SEEK_SET);

    /* Separate the DDS from data*/
    stat = oc_extract(state);
    if(stat != OC_NOERR) goto unwind;
    dxd->text = ocbdup(state->dap.packet);
    ocbclear(state->dap.packet);

    /* reset the position of the tmp file*/
    fseek(state->dap.file,state->dap.bod,SEEK_SET);

    /* Parse the dds*/
    dxd->nodes = oclistnew();
    dxd->tree = DAPparse(state,dxd->text,dxd->nodes);
    if(dxd->tree == NULL) {
	oclog(LOGERR,"oc_open:DATA DDS parse failed\n");
	stat = OC_EINVAL;
	goto unwind;
    }

    if(dxd->tree->octype != OC_Dataset) {
	oclog(LOGERR,"oc_open:URL is not a legal DDS\n");
	return THROW(OC_EINVAL);
    }

    dxd->tree->datadds = 1; /* data dds*/

    /* Process ocnodes to fix various semantic issues*/
    computeocsemantics(dxd->nodes);

    /* Process ocnodes to compute name info*/
    computefullnames(dxd->tree);

    /* Process ocnodes to compute sizes for those that are uniform in size*/
    ocsetsize(dxd->tree);

    state->dap.xdrs = (XDR*)ocmalloc(sizeof(XDR));
    MEMCHECK(state->dap.xdrs,OC_ENOMEM);
    ocxdrstdio_create(state->dap.xdrs,state->dap.file,XDR_DECODE);
    if(!xdr_setpos(state->dap.xdrs,state->dap.bod)) return xdrerror();

    DEBUG(2,"DDS dump:");
    DEBUGTEXT(2,dxd->text);

unwind:
	if(state != NULL) {
	    /* TBD
	       clean up state and ocfree it
	       release curl*/
	}
    return THROW(stat);
}

void
oc_close(OCconnection* state)
{
    if(state == NULL) return;

    oc_dataddsclear(state);
    oc_ddsclear(state);
    oc_dasclear(state);

    if(state->dap.xdrs != NULL) {
	xdr_destroy(state->dap.xdrs);
	ocfree(state->dap.xdrs);
    }
    dapurlclear(&state->dap.url);

    if(state->dap.originalurl != NULL) ocfree(state->dap.originalurl);
    if(state->dap.packet != NULL) ocbfree(state->dap.packet);
    if(state->dap.file != NULL) fclose(state->dap.file);
    if(state->dap.filename != NULL) {
	unlink(state->dap.filename);
	free(state->dap.filename);
    }
    if(state->dap.projections != NULL) {
        while(oclistlength(state->dap.projections) > 0U) {
	    Projectionclause* pcl = (Projectionclause*)oclistpop(state->dap.projections);
	    ocfreeprojectionclause(pcl);
	}
	oclistfree(state->dap.projections);
    }
    if(state->map.sizes != NULL) oclistfree(state->map.sizes);
    if(state->content.memdata != NULL) freeocmemdata(state->content.memdata);
    if(state->content.contentlist != NULL) {
	struct OCcontent* next;
	struct OCcontent* curr = state->content.contentlist;
	while(curr != NULL) {
	    next = curr->next;
	    ocfree(curr);
	    curr = next;
	}
    }
    if(state->dap.curl != NULL) curlclose(state->dap.curl);
    ocfree(state);
}


static int
oc_extract(OCconnection* state)
{
    /* Read until we find the separator (or EOF)*/
    ocbclear(state->dap.packet);
    fseek(state->dap.file,0,SEEK_SET);
    for(;;) {
	char chunk[128];
	size_t count;
	size_t ddslen, bod;
	/* read chunks of the file until we find the separator*/
        count = fread(chunk,1,sizeof(chunk),state->dap.file);
	if(count <= 0)  { /* EOF;*/
	    break;
	}
        ocbappendn(state->dap.packet,chunk,sizeof(chunk));
	if(findbod(state->dap.packet,&bod,&ddslen)) { /* found it*/
	    state->dap.bod = bod;
	    ocbsetlength(state->dap.packet,ddslen);
	    return THROW(OC_NOERR);
	}
    }
    return THROW(OC_EDAP);
}
