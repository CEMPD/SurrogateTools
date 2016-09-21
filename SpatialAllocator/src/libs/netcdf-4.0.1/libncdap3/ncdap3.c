/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdap3.c,v 1.20 2009/03/25 17:11:32 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"
#include "dispatch3.h"
#include "dapdispatch3.h"
#include "dapalign.h"
#include "daprename.h"

static int initialized = 0;

/*Forward*/
static NCerror buildncstructures(NC* ncp);
static NCerror builddims(NC* ncp, NClist*);
static NCerror buildvars(NC* ncp, NClist*);
static NCerror buildglobalattrs(NC* ncp, CDFnode* root);
static NCerror buildattribute1(Attribute* att, int varid, int ncid);
static void applyclientparams(NCDRNO* drno);
static OCerror computeunlimitedsize(NCDRNO* drno, size_t*);
static void definestringdims(NCDRNO* drno);
static void defineunlimited(NCDRNO* drno);
static void initialize(void);

#define getncid(ncp) ((ncp)->nciop->fd)

/**************************************************/
/* Add an extra function whose sole purpose is to allow
   configure(.ac) to test for the presence of thiscode.
*/
int nc__opendap(void) {return 0;}

/**************************************************/

int
nc3d_open(const char* path, int mode, int* ncidp)
{
    NC *ncp = NULL;
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    NCDRNO* drno = NULL;
    size_t count;

    if(!initialized) {
	initialize();
    }

    /* Setup tentative DRNO state*/
    drno = (NCDRNO*)emalloc(sizeof(NCDRNO));
    MEMCHECK(drno,NC_ENOMEM);
    drno->dap.urltext = strdup(path);

    if(!dapurlparse(path,&drno->dap.url)) {
	dapurlclear(&drno->dap.url);
	ncstat = lnc3__open_mp(path,mode,0,NULL,ncidp);
	if(ncstat == NC_NOERR) setnc3dispatch(*ncidp);
	return THROW(ncstat);
    }

    /* Set up the ncp structure*/
    ncp = drno_new_NC(NULL);
    if(ncp == NULL) {ncstat = THROW(NC_ENOMEM); goto unwind;}
    ncp->dispatch = &ncdap3lib;
    ncp->drno = drno;
    drno->ncp = ncp; /* cross link*/
    drno->cdf.cdfnodes = nclistnew();    
    /* Mark as indef state*/
    fSet(ncp->flags, NC_INDEF);

    /* Presume a DAP URL*/
    ocstat = oc_open(&drno->dap.conn);
    if(ocstat != OC_NOERR) goto unwind;

    /* Use the oc tmp file as our ncio*/
    /* Should not need exclusive access*/
    /* as the file is already open with*/
    /* user access only.*/
    /* mode = (NC_NOCLOBBER);*/
    mode = 0;
    ncstat = drno_ncio_open(ncp,drno->dap.conn->dap.filename,mode);
    if(ncstat != NC_NOERR) goto unwind;

    /* Obtain the pure DDS and the pure DAS*/
    ocstat = oc_fetchdds(drno->dap.conn,path);
    if(ocstat != OC_NOERR) goto unwind;

    /* Get DAS, but ignore if fails*/
    /* Side effect: merge with previously fetched dds*/
    ocstat = oc_fetchdas(drno->dap.conn,path);

    /* Construct our parallel dds tree*/
    drno->cdf.root = buildcdftree3(drno,oc_getdds(drno->dap.conn),NULL);

    /* Compute misc. node info*/
    computecdfinfo3(drno,drno->cdf.cdfnodes);

    /* apply client parameters (after computcdfinfo)*/
    applyclientparams3(drno);

    /* Add unlimited dimension*/
    defineunlimited(drno);

    /* Add (as needed) string dimensions*/
    definestringdims(drno);

    /* Re-compute the dimension names*/
    computecdfdimnames3(drno->cdf.cdfnodes);

    /* Re-compute the var names*/
    computecdfvarnames3(drno);

    /* Make sure this ncp is visible before defining nc schema*/
    drno_add_to_NCList(ncp);

    ncstat = buildncstructures(ncp);
    if(ncstat != NC_NOERR) {
        drno_del_from_NCList(ncp); /* undefine here */
	goto unwind;
    }

    /* Mark as no longer indef */
    fClr(ncp->flags, NC_INDEF);

    /* Compute the unlimited size if requested*/
    computeunlimitedsize(drno,&count);
    drno_set_numrecs(ncp,count);

    *ncidp = ncp->nciop->fd;
    return THROW(NC_NOERR);

unwind:
	if(ncp != NULL) {
	    ncio_close(ncp->nciop,1);
	    ncp->nciop = NULL;
   	    drno_free_NC(ncp);
	}
	if(drno != NULL) freeNCDRNO3(drno);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

int
nc3d_close(int ncid)
{
    int ncstatus = NC_NOERR;
    NCDRNO* drno;
    NC *ncp; 

    ncstatus = NC_check_id(ncid, &ncp); 
    if(ncstatus != NC_NOERR) return THROW(ncstatus);

    drno = ncp->drno;
    freeNCDRNO3(drno);

    (void) ncio_close(ncp->nciop, 0); /* should also close the fd*/
    ncp->nciop = NULL;

    drno_del_from_NCList(ncp);
    drno_free_NC(ncp);

    return THROW(ncstatus);
}

static void
initialize(void)
{
    initialized = 1;
    compute_nccalignments();
}

void
freeNCDRNO3(NCDRNO* drno)
{
    if(drno->var.varinfo != NULL) free(drno->var.varinfo);
    if(drno->cdf.cdfnodes != NULL) {freeallcdfnodes3(drno->cdf.cdfnodes); nclistfree(drno->cdf.cdfnodes);}
    if(drno->cdf.ocmap != NULL) nchashfree(drno->cdf.ocmap);
    dapurlclear(&drno->dap.url);
    if(drno->dap.urltext != NULL) free(drno->dap.urltext);
    oc_close(drno->dap.conn);
    free(drno);
}


/* Construct the pseudo-unlimited dimension*/
/* See if the schema is such that it can support unlimited access*/
/* Must be exactly one top-level sequence node and it must be*/
/* a direct child of the dataset node.*/
/* Further, there must be no other sequence in the schema*/
/* Apply [limit=...] from client parameters.*/
/* May relax conditions some day.*/

static void
defineunlimited(NCDRNO* drno)
{
    unsigned int i;
    unsigned int count = 0;
    unsigned int index = 0;
    CDFnode* seq = NULL;
    CDFnode* dataset = drno->cdf.root;
    ASSERT((dataset->nctype == NC_Dataset));
    /* First, locate candidate sequence*/
    for(i=0;i<nclistlength(dataset->subnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(dataset->subnodes,i);
	if(node->nctype == NC_Sequence) {count++; seq=node; index = i;}
    }
    /* Now check that it is the only sequence*/
    if(count == 1) {
	count = 0;
        for(i=0;i<nclistlength(drno->cdf.cdfnodes);i++) {
	    CDFnode* node = (CDFnode*)nclistget(drno->cdf.cdfnodes,i);
	    if(node->nctype == NC_Sequence && node != seq) {count = 1; break;}
	}
	if(count == 0) { /* only one sequence*/
	    drno->cdf.sequnlimited = seq;
	    drno->cdf.sequnlimitedfield = index;
	}
    }
    drno->cdf.unlimited = makecdfnode3(drno,"unlimited",OC_Dimension,NULL,NULL);
    drno->cdf.unlimited->ncbasename = cdfbasename(drno->cdf.unlimited->name);
}

static void
definestringdims(NCDRNO* drno)
{
    /* for all variables of string type, we will need another dimension*/
    /* to represent the string; Accumulate the needed sizes and create the dimensions*/
    /* with a specific name: stringdimNNN*/
    int i;
    NClist* cdfnodes = drno->cdf.cdfnodes;
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(cdfnodes,i);
	char tmp[64];
	CDFnode* sdim = NULL;
	if(var->nctype != NC_Primitive) continue;
	if(var->etype != NC_STRING && var->etype != NC_URL) continue;
	/* create a psuedo dimension for the charification of the string*/
	snprintf(tmp,sizeof(tmp),"stringdim%lu",
			(unsigned long)var->maxstringlength);
	sdim = makecdfnode3(drno, tmp, OC_Dimension, NULL, var);
        sdim->container = var;
	sdim->dim.declsize = var->maxstringlength;
	/* Do the work of computecdfinfo to define cdfname and fullcdfname*/
	sdim->ncbasename = cdfbasename(sdim->name);
        /* sdim->fullcdfname = strdup(sdim->cdfname);*/
	/* tag the variable with its string dimension*/
	nclistpush(var->array.dimensions,(ncelem)sdim);
	var->array.rank++;
    }
}

static NCerror
buildncstructures(NC* ncp)
{
    NCerror ncstat = NC_NOERR;
    NCDRNO* drno = ncp->drno;
    CDFnode* dds = drno->cdf.root;
    NClist* ddsall = drno->cdf.cdfnodes;
    ncstat = buildglobalattrs(ncp,dds);
    if(ncstat != NC_NOERR) goto fail;
    ncstat = builddims(ncp,ddsall);
    if(ncstat != NC_NOERR) goto fail;
    ncstat = buildvars(ncp,ddsall);
    if(ncstat != NC_NOERR) goto fail;
fail:
    return THROW(ncstat);
}

static NCerror
builddims(NC* ncp, NClist* cdfnodes)
{
    int i;
    NCerror ncstat = NC_NOERR;
    int dimid;
    int ncid = getncid(ncp);
    NCDRNO* drno = ncp->drno;

    ASSERT((cdfnodes != NULL));

    /* Construct the unlimited dimension first*/
    ncstat = nc_def_dim(ncid,
			drno->cdf.unlimited->name,
			NC_UNLIMITED,
			&drno->cdf.unlimited->ncid);
    if(ncstat != NC_NOERR) goto fail;

    /* Now, go in and set the effective size of UNLIMITED to 0;*/
    /* this cannot be done thru the normal API.*/
    drno_set_numrecs(ncp,0);
	
    /* define all other dimensions*/
    /* taking basedim info into account*/
    /* (see computecdfdimnames)*/
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* dim = (CDFnode*)nclistget(cdfnodes,i);
	if(dim->nctype != NC_Dimension) continue;
	if(dim->dim.basedim != NULL) continue;
	if(dim == drno->cdf.unlimited) continue; /* already defined*/
        ncstat = nc_def_dim(ncid,dim->ncfullname,dim->dim.declsize,&dimid);
        if(ncstat != NC_NOERR) return THROW(ncstat);
        dim->ncid = dimid;
    }
    /* Make all duplicate dims have same dimid as basedim*/
    /* (see computecdfdimnames)*/
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* dim = (CDFnode*)nclistget(cdfnodes,i);
	if(dim->nctype != NC_Dimension) continue;
	if(dim->dim.basedim != NULL) dim->ncid = dim->dim.basedim->ncid;
    }
fail:
    return THROW(ncstat);
}

/* Simultaneously build any associated attributes*/
/* and any necessary pseudo-dimensions for string types*/
static NCerror
buildvars(NC* ncp, NClist* cdfnodes)
{
    int i,j,dimindex;
    NCerror ncstat = NC_NOERR;
    int varid;
    int ncid = getncid(ncp);
    NClist* path = nclistnew();
    NClist* dimset = nclistnew();
    NCDRNO* drno = ncp->drno;

    ASSERT((cdfnodes != NULL));
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(cdfnodes,i);
        int ndims;
        int* dimids = NULL;

	if(var->nctype != NC_Primitive) continue;

	/* collect relevant containers into two parts*/
        nclistclear(path);
        nclistclear(dimset);
        collectnodepath(var,path);
        collectdims(drno,path,dimset);

	ndims = nclistlength(dimset);
        dimids = (int*)ecalloc(sizeof(int),ndims);
	MEMCHECK(dimids,NC_ENOMEM);
	dimindex = 0;
	if(ndims > 0) {
            for(j=0;j<ndims;j++) {
                CDFnode* dim = (CDFnode*)nclistget(dimset,j);
                dimids[dimindex++] = dim->ncid;
 	    }
        }   
	ASSERT((ndims == dimindex));
        ncstat = nc_def_var(ncid,var->ncfullname,
                        var->externaltype,
                        ndims,
                        dimids,
                        &varid);
        if(dimids != NULL) free(dimids);
        if(ncstat != NC_NOERR) goto fail;
        var->ncid = varid;
	if(var->dds->attributes != NULL) {
	    for(j=0;j<oclistlength(var->dds->attributes);j++) {
		Attribute* att = (Attribute*)oclistget(var->dds->attributes,j);
		ncstat = buildattribute1(att,varid,ncid);
        	if(ncstat != NC_NOERR) goto fail;
	    }
	}
    }    
fail:
    nclistfree(dimset);
    nclistfree(path);
    return THROW(ncstat);
}

static NCerror
buildglobalattrs(NC* ncp, CDFnode* root)
{
    int i;
    NCerror ncstat = NC_NOERR;
    int ncid = getncid(ncp);
    NCDRNO* drno = ncp->drno;
    const char* txt;
    char *nltxt, *p;

    if(root->dds->attributes != NULL) {
        for(i=0;i<oclistlength(root->dds->attributes);i++) {
   	    Attribute* att = (Attribute*)oclistget(root->dds->attributes,i);
	    ncstat = buildattribute1(att,NC_GLOBAL,ncid);
            if(ncstat != NC_NOERR) goto fail;
	}
    }
    /* Define some additional system global attributes depending on show= clientparams*/
    /* Ignore failures*/
    if(showcheck(drno,"url")) {
	if(drno->dap.urltext != NULL)
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_URL",
				       strlen(drno->dap.urltext),drno->dap.urltext);
    }
    if(showcheck(drno,"dds")) {
	txt = oc_getddstext(drno->dap.conn);
	if(txt != NULL) {
	    /* replace newlines with spaces*/
	    nltxt = strdup(txt);
	    for(p=nltxt;*p;p++) {if(*p == '\n' || *p == '\r' || *p == '\t') {*p = ' ';}};
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_DDS",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }
    if(showcheck(drno,"das")) {
	txt = oc_getdastext(drno->dap.conn);
	if(txt != NULL) {
	    nltxt = strdup(txt);
	    for(p=nltxt;*p;p++) {if(*p == '\n' || *p == '\r' || *p == '\t') {*p = ' ';}};
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_DAS",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }
    if(showcheck(drno,"datadds")) {
	txt = oc_getdataddstext(drno->dap.conn);
	if(txt != NULL) {
	    nltxt = strdup(txt);
	    for(p=nltxt;*p;p++) {if(*p == '\n' || *p == '\r' || *p == '\t') {*p = ' ';}};
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_DATADDS",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }

fail:
    return THROW(ncstat);
}

static NCerror
buildattribute1(Attribute* att, int varid, int ncid)
{
    NCerror ncstat = NC_NOERR;
    char* cname = cdfbasename(att->name);
    /* If the type of the attribute is string, then we need*/
    /* to convert to a single character string*/
    if(att->etype == NC_STRING || att->etype == NC_URL) {
        int k;
	char** strings = (char**)att->values;
	char* newstring;
	size_t newlen = 0;
	for(k=0;k<att->nvalues;k++) {newlen += strlen(strings[k]);}
	newstring = (char*)emalloc(newlen+1);
        MEMCHECK(newstring,NC_ENOMEM);
	newstring[0] = '\0';
	for(k=0;k<att->nvalues;k++) {strcat(newstring,strings[k]);}
	ncstat = nc_put_att(ncid,varid,cname,NC_CHAR,newlen,newstring);
	free(newstring);
    } else {
        ncstat = nc_put_att(ncid,varid,cname,
			    att->etype,att->nvalues,att->values);
    }
    free(cname);
    return THROW(ncstat);
}

void
applyclientparams3(NCDRNO* drno)
{
    int i,len;
    NClist* cdfnodes;
    int dfaltstrlen = DEFAULTSTRINGLENGTH;
    int dfaltseqlim = 0;
    const char* value;

    value = daplookup(drno,"stringlength");
    if(value != NULL && strlen(value) != 0) {
        if(sscanf(value,"%d",&len) && len > 0) dfaltstrlen = len;
    }
    value = daplookup(drno,"limit");
    if(value != NULL && strlen(value) != 0) {
        if(sscanf(value,"%d",&len) && len > 0) dfaltseqlim = len;
    }
    drno->cdf.defaultstringlength = dfaltstrlen;
    drno->cdf.defaultsequencelimit = dfaltseqlim;

    cdfnodes = drno->cdf.cdfnodes;
    for(i=0;i<nclistlength(cdfnodes);i++) {
        int len;
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
	char tmpname[NC_MAX_NAME+32];
	const char* value;

	if(node->nctype != NC_Primitive) continue;
	/* Define the client param stringlength for this variable*/
	node->maxstringlength = dfaltstrlen; /* unless otherwise stated*/
	node->maxsequencelength = dfaltseqlim;
	strcpy(tmpname,"stringlength_");
	strcat(tmpname,node->dds->fullname);
	value = daplookup(drno,tmpname);	
        if(value != NULL && strlen(value) != 0) {
            if(sscanf(value,"%d",&len) && len > 0) node->maxstringlength = len;
	}
	strcpy(tmpname,"limit_");
	strcat(tmpname,node->dds->fullname);
	value = daplookup(drno,tmpname);	
        if(value != NULL && strlen(value) != 0) {
            if(sscanf(value,"%d",&len) && len > 0) node->maxsequencelength = len;
	}
    }
}

/* See if the schema is such that it can support unlimited access*/
/* Must be exactly one top-level sequence node and it must be*/
/* a direct child of the dataset node.*/
/* Further, there must be no other sequence in the schema*/
/* Apply [limit=...] from client parameters.*/
/* May relax conditions some day.*/

static OCerror
computeunlimitedsize(NCDRNO* drno, size_t* countp)
{
    OCerror ocstat = OC_NOERR;
    OCcontent* rootcontent = NULL;
    OCcontent* seqcontent = NULL;
    OCconnection* conn = drno->dap.conn;
    size_t count = 0;

    if(drno->cdf.sequnlimited == NULL) goto done; /* there is no top-level sequence*/
    if(!daplookup(drno,"unlimitedsequence")) goto done;

    /* reset state*/
    oc_dataddsclear(conn);
    detachdatadds3(drno);    

    /* Obtain the datadds*/
    ocstat = oc_fetchdatadds(conn,drno->dap.urltext);
    if(ocstat != OC_NOERR) goto fail;

    /* Attach datadds to our node set*/
    attachdatadds3(drno->cdf.root,oc_getdatadds(conn));

    /* Get root content*/
    rootcontent = oc_newcontent(conn);
    ocstat = oc_rootcontent(conn,rootcontent);
    if(ocstat != OC_NOERR) goto fail;
    /* Move to the unlimited sequence*/
    seqcontent = oc_newcontent(conn);
    ocstat = oc_fieldcontent(conn,rootcontent,seqcontent,drno->cdf.sequnlimitedfield);
    if(ocstat != OC_NOERR) goto fail;
    /* compute the number of records*/
    count = oc_recordcount(conn,seqcontent);

    if(drno->cdf.defaultsequencelimit > 0 && drno->cdf.defaultsequencelimit < count)
        count = drno->cdf.defaultsequencelimit;
    goto done;

fail:
    count = 0;

done:
    oc_dataddsclear(conn);
    if(seqcontent != NULL) oc_freecontent(conn,seqcontent);
    if(rootcontent != NULL) oc_freecontent(conn,rootcontent);
    drno->cdf.unlimited->dim.declsize = count;
    if(countp) *countp = count;
    return ocstat;
}

void
detachdatadds3(NCDRNO* drno)
{
    int i;
    for(i=0;i<nclistlength(drno->cdf.cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(drno->cdf.cdfnodes,i);
	node->datadds = NULL;
   }
}

void
attachdatadds3(CDFnode* cdfnode, OCnode* ocnode)
{
    int i,j;
    ASSERT((cdfnode->nctype == octypetonc(ocnode->octype)));
    ASSERT((strcmp(cdfnode->name,ocnode->name)==0));
    ASSERT((nclistlength(ocnode->subnodes) <= nclistlength(cdfnode->subnodes)));
    cdfnode->datadds = ocnode;
    for(i=0;i<oclistlength(ocnode->subnodes);i++) {
	OCnode* ocsub = (OCnode*)oclistget(ocnode->subnodes,i);
	int found = 0;
        for(j=0;j<nclistlength(cdfnode->subnodes);j++) {
	    CDFnode* cdfsub = (CDFnode*)nclistget(cdfnode->subnodes,j);
	    if(strcmp(cdfsub->name,ocsub->name)==0) {
		attachdatadds3(cdfsub,ocsub);
		found = 1;
	        break;
	    }
	}
	if(!found) PANIC1("attachdatadds: unattached datadds node: %s",ocsub->name);
    }
}

/* Check to see if a specific show= clientparam is specified*/
int
showcheck(NCDRNO* drno, const char* showparam)
{
    const char* show;
    const char* sh;
    unsigned int splen;
    if(drno == NULL || showparam == NULL) return 0;
    show = daplookup(drno,"show");
    if(show == NULL) return 0;
    splen = strlen(showparam);
    for(sh=show;*sh;sh++) {
	if(strncmp(sh,showparam,splen)==0) {
	    if(sh != show && *(sh-1) != '+') continue;
	    if(sh[splen] != '\0' && sh[splen] != '+') continue;
	    return 1;
	}
    }
    return 0;
}

/* Wrap clientparam lookup to isolate where the params come from */
const char*
daplookup(NCDRNO* drno, const char* param)
{
    return dapurllookup(&drno->dap.url,param);
}
