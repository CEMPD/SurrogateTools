/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/cdf3.c,v 1.2 2009/03/30 17:03:55 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"

#define CDFPATHSEPARATOR "_"

/* Forward*/
static void computecdfnctype(CDFnode*);
static void computecdfattributes(CDFnode* node);
static void dupdimensions(OCnode* ocnode, CDFnode* cdfnode, NCDRNO* drno);
static NClist* hoist(CDFnode* container);
static void makevarnames(NClist* vars);

/* Given an OCnode tree, mimic it as a CDFnode tree*/
CDFnode*
buildcdftree3(NCDRNO* drno, OCnode* ocnode, CDFnode* container)
{
    CDFnode* cdfnode = NULL;
    int i;

    if(drno->cdf.ocmap == NULL) {drno->cdf.ocmap = nchashnew();} /* ensure its existence*/

    switch (ocnode->octype) {
    case OC_Dataset:
    case OC_Grid:
    case OC_Structure:
    case OC_Sequence:
    case OC_Primitive:
	cdfnode = makecdfnode3(drno,ocnode->name,ocnode->octype,ocnode,container);
	break;

    case OC_Dimension:
    default: PANIC1("buildcdftree: unexpect OC node type: %d",ocnode->octype);
    }    
    if(ocnode->array.rank > 0) dupdimensions(ocnode,cdfnode,drno);
    for(i=0;i<oclistlength(ocnode->subnodes);i++) {
	OCnode* ocsubnode = (OCnode*)oclistget(ocnode->subnodes,i);
	CDFnode* subnode = buildcdftree3(drno,ocsubnode,cdfnode);
	nclistpush(cdfnode->subnodes,(ncelem)subnode);
    }
    return cdfnode;
}

/* public version*/
CDFnode*
makecdfnode3(NCDRNO* drno, char* name, OCtype octype, /*optional*/ OCnode* ocnode, CDFnode* container)
{
    CDFnode* node;
    assert(drno != NULL);
    node = (CDFnode*)emalloc(sizeof(CDFnode));
    if(node == NULL) return (CDFnode*)NULL;
    node->name = nulldup(name);
    node->nctype = octypetonc(octype);
    node->dds = ocnode;
    node->owner = drno;
    node->subnodes = nclistnew();
    node->array.dimensions = nclistnew();
    node->container = container;
    nclistpush(drno->cdf.cdfnodes,(ncelem)node);
    if(ocnode != NULL) {
        node->etype = octypetonc(ocnode->etype);
	nchashinsert(drno->cdf.ocmap,(nchashid)ocnode,(ncelem)node); /* map ocnode -> cdfnode*/
    }
    return node;
}

static void
dupdimensions(OCnode* ocnode, CDFnode* cdfnode, NCDRNO* drno)
{
    int i;
    assert(ocnode->array.rank > 0);
    for(i=0;i<ocnode->array.rank;i++) {
	CDFnode* cdfdim;
	OCnode* ocdim = (OCnode*)oclistget(ocnode->array.dimensions,i);
	cdfdim = makecdfnode3(drno,ocdim->name,ocdim->octype,ocdim,cdfnode);
	cdfdim->dim.declsize = ocdim->dim.declsize;
	nclistpush(cdfnode->array.dimensions,(ncelem)cdfdim);
    }    
    cdfnode->array.rank = nclistlength(cdfnode->array.dimensions);    
}

void
freeallcdfnodes3(NClist* nodes)
{
    unsigned int i;
    for(i=0;i<nclistlength(nodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(nodes,i);
        if(node->name != NULL) free(node->name);
        if(node->ncbasename != NULL) free(node->ncbasename);
        if(node->ncfullname != NULL) free(node->ncfullname);
        nclistfree(node->array.dimensions);
        nclistfree(node->subnodes);
        efree(node);
    }
}

void
computecdfinfo3(NCDRNO* drno, NClist* cdfnodes)
{
    int i;
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
        if(node->nctype == NC_Dimension && node->name == NULL) continue;
        if(node->ncbasename != NULL) {free(node->ncbasename); node->ncbasename = NULL;}
        if(node->name != NULL) node->ncbasename = cdfbasename(node->name);
    }
#ifdef IGNORE
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
        if(node->nctype == NC_Dimension && node->name == NULL) continue;
        computefullname(node);
    }
#endif
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
        computecdfnctype(node);
    }
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
	if(node->nctype != NC_Dimension) continue;
	node->maxstringlength = drno->cdf.defaultstringlength;
        computecdfattributes(node);
    }
}

#ifdef IGNORE
static void
computebasename(CDFnode* node)
{
    if(node->ncbasename != NULL) {free(node->ncbasename); node->ncbasename = NULL;}
    if(node->name != NULL) node->ncbasename = cdfbasename(node->name);
 }

static void
computefullname3(CDFnode* node)
{
    char* tmp;
    char* fullname;
    NClist* path;

    if(node->fullname != NULL) {free(node->fullname); node->fullname = NULL;}
    if(node->ncbasename == NULL) return;
    path = nclistnew();
    collectnodepath(node->container,path);
    tmp = cdfpathstring(path,CDFPATHSEPARATOR);
    if(tmp == NULL) {
        fullname = strdup(node->ncbasename);
    } else {
        fullname = (char*)emalloc(strlen(tmp)
                                 +strlen(CDFPATHSEPARATOR)
                                 +strlen(node->ncbasename)
                                 +1);
	if(fullname == NULL) return;
        strcpy(fullname,tmp);
        strcat(fullname,CDFPATHSEPARATOR);
        strcat(fullname,node->ncbasename);
        free(tmp);
    }
    node->fullname = fullname;
}
#endif

static void
computecdfnctype(CDFnode* node)
{
    if(node->nctype == NC_Primitive)
        node->externaltype = nctypeupgrade(node->etype);
}

static void
computecdfattributes(CDFnode* node)
{
    NClist* path = nclistnew();
    if(node->nctype == NC_Primitive) {
        /* Add in copies of the attributes of all parent nodes*/
        collectnodepath(node->container,path);
    }
    nclistfree(path);
}

/* Provide short names for dimensions*/
/* Assume that computebasenames and*/
/* computefullnames have been called*/
void
computecdfdimnames3(NClist* cdfnodes)
{
    int i,j,stable;
    CDFnode* root;
    NClist* dims = nclistnew();
    char tmp[NC_MAX_NAME];

    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
	if(node->nctype == NC_Dataset) root = node;
	if(node->nctype != NC_Dimension) continue;
	nclistpush(dims,(ncelem)node);
    }

    /* Start by assigning names to anonymous dimensions*/
    for(i=0;i<nclistlength(dims);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dims,i);
	if(dim->name != NULL) continue;
	snprintf(tmp,sizeof(tmp),"DIM%lu",(unsigned long)dim->dim.declsize);
	if(dim->ncbasename != NULL) free(dim->ncbasename);
	if(dim->ncfullname != NULL) free(dim->ncfullname);
	dim->name = strdup(tmp);
	dim->ncbasename = cdfbasename(dim->name);
	dim->ncfullname = nulldup(dim->ncbasename);
    }	

    /* handle the easy case where two dims have same name and sizes*/
    /* make the second and later ones point to the leader dimension*/
    for(i=0;i<nclistlength(dims);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dims,i);
	if(dim->dim.basedim != NULL) continue; /* already processed*/
	for(j=0;j<i;j++) {
	    CDFnode* basedim = (CDFnode*)nclistget(dims,j);
	    if(dim->dim.basedim != NULL) continue; /* already processed	*/
	    if(strcmp(dim->name,basedim->name) == 0
	       && dim->dim.declsize == basedim->dim.declsize) {
	        dim->dim.basedim = basedim; /* same name and size*/
	    }
	}
    }

    /* Rename all remaining dimensions to be top_level with short names*/
    for(i=0;i<nclistlength(dims);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dims,i);
	if(dim->dim.basedim != NULL) continue; /* already processed*/
	if(dim->ncbasename != NULL) free(dim->ncbasename);
	if(dim->ncfullname != NULL) free(dim->ncfullname);
	dim->ncbasename = cdfbasename(dim->name);
	dim->ncfullname = strdup(dim->ncbasename);
    }

    /* Remaining case: same name and different sizes*/
    /* => rename second dim.*/
    /* May require multiple passes to stabilize*/
    do {
	stable = 1; /* assume so until told otherwise*/
        for(i=0;i<nclistlength(dims);i++) {
	    CDFnode* dim = (CDFnode*)nclistget(dims,i);
	    if(dim->dim.basedim != NULL) continue; /* ignore*/
	    for(j=0;j<i;j++) {
	        CDFnode* basedim = (CDFnode*)nclistget(dims,j);
	        if(basedim->dim.basedim != NULL) continue; /* ignore		*/
		if(strcmp(dim->name,basedim->name)!=0) continue;
		if(dim->dim.declsize == basedim->dim.declsize) continue;
		/* Ok, we have two dims with same name and different sizes => rename*/
		snprintf(tmp,sizeof(tmp),"%s%lu",
			 dim->name,(unsigned long)dim->dim.declsize);
		if(dim->name != NULL) free(dim->name);
		if(dim->ncbasename != NULL) free(dim->ncbasename);
		if(dim->ncfullname != NULL) free(dim->ncfullname);
		dim->name = strdup(tmp);
		dim->ncbasename = cdfbasename(dim->name);
		dim->ncfullname = strdup(dim->ncbasename);
		stable = 0; /* need to check again for duplicate names*/
	    }
 	}
    } while(!stable);

    /* Finally, verify unique names for dimensions*/
    for(i=0;i<nclistlength(dims);i++) {
	CDFnode* dim1 = (CDFnode*)nclistget(dims,i);
	if(dim1->dim.basedim != NULL) continue;
	for(j=0;j<i;j++) {
	    CDFnode* dim2 = (CDFnode*)nclistget(dims,j);
	    if(strcmp(dim1->ncbasename,dim2->ncbasename)==0) {
		PANIC1("duplicate names: %s",dim1->ncbasename);
	    }
	}
    }
    /* clean up*/
    nclistfree(dims);
    /* Remove elided mark */
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
	node->elided = 0;
    }
}

/* Attempt to hoist var names to shorten them.*/
/* Assume that computebasenames and*/
/* computefullnames have been called*/
void
computecdfvarnames3(NCDRNO* drno)
{
    NClist* vars;
    int i;
    drno->cdf.root->elided = 1;
    /* ensure that all variables have an initial full path name defined*/
    for(i=0;i<nclistlength(drno->cdf.cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(drno->cdf.cdfnodes,i);
	if(node->nctype == NC_Primitive) {
	    if(node->ncfullname != NULL) efree(node->ncfullname);
	    node->ncfullname = makecdfpathstring3(node);
	}
    }
    vars = hoist(drno->cdf.root);
    nclistfree(vars);
}

static NClist*
hoist(CDFnode* container)
{
    int i,j;
    NClist* containers = nclistnew();
    NClist* unique = nclistnew();
    /* Divide the subnodes of this node into containers and unique*/
    for(i=0;i<nclistlength(container->subnodes);i++) {
	CDFnode* sub = (CDFnode*)nclistget(container->subnodes,i);
	switch (sub->nctype) {
	case NC_Structure:
	case NC_Sequence:
	case NC_Grid:
	case NC_Dataset:
	    nclistpush(containers,(ncelem)sub);
	    break;
	case NC_Primitive:
	    nclistpush(unique,(ncelem)sub);
	    break;
	default: PANIC1("computecdfvarnames: bad node type: %d",sub->nctype);
	}
    }

    /* Tentatively hoist each container in turn*/
    while(nclistlength(containers) > 0) {
	CDFnode* subcontainer = (CDFnode*)nclistremove(containers,0);	
	NClist* vars = hoist(subcontainer);
	int match;
	/* compute path names without this container*/
	subcontainer->elided = 1;
	if(ncdap3debug > 1) fprintf(stderr,"eliding: %s\n",subcontainer->name);
	makevarnames(vars);
	/* look for duplicates in the unique list*/
	match = 0;
        for(i=0;i<nclistlength(unique);i++) {
	    CDFnode* unode = (CDFnode*)nclistget(unique,i);
            for(j=0;j<nclistlength(vars);j++) {
	        CDFnode* var = (CDFnode*)nclistget(vars,j);
		if(strcmp(var->ncfullname,unode->ncfullname)==0) {
		    match = 1;
		    if(ncdap3debug > 1) fprintf(stderr,"match: %s\n",var->ncfullname);
		    break;
		}
	    }
	    if(match) break;
	}
	if(match) {
	    /* Since our goal is to hoist all the vars in a compound type or none,*/
            /* match => we have a collision, so restore the path name of the vars*/
	    /* to include their container*/
	    subcontainer->elided = 0;
	    makevarnames(vars);
	}
	/* Add the subcontainer vars to our list of uniquely named vars*/
        for(i=0;i<nclistlength(vars);i++) {
	    CDFnode* var = (CDFnode*)nclistget(vars,i);
	    nclistpush(unique,(ncelem)var);
	}
	nclistfree(vars);
    }
    nclistfree(containers);
    return unique;
}

static void
makevarnames(NClist* vars)
{
    int i;
    for(i=0;i<nclistlength(vars);i++) {
	CDFnode* var = (CDFnode*)nclistget(vars,i);
	if(var->ncfullname != NULL) efree(var->ncfullname);
        var->ncfullname = makecdfpathstring3(var);
	if(ncdap3debug > 1)
	    fprintf(stderr,"makevarname: %s->ncfullname=%s\n",var->name,var->ncfullname);
    }
}

/* Convert a path to a name string; elide the initial Dataset node*/
/* and elide any node marked as elided.*/
char*
makecdfpathstring3(CDFnode* var)
{
    int slen,i,len,first;
    char* pathname;
    NClist* path = nclistnew();

    collectnodepath(var,path);
    len = nclistlength(path);
    assert(len > 1); /* dataset plus primitive*/
    for(slen=0,i=0;i<len;i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	if(node->nctype == NC_Dataset) continue;
	slen += strlen(node->ncbasename);
    }
    slen += ((len-2)*strlen(CDFPATHSEPARATOR));
    slen += 1;   /* for null terminator*/
    pathname = (char*)emalloc(slen);
    MEMCHECK(pathname,NULL);
    pathname[0] = '\0';    
    for(first=1,i=0;i<len;i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	char* name = node->ncbasename;
	if(node->nctype == NC_Dataset) continue;
	if(node->elided) continue;
	if(!first) strcat(pathname,CDFPATHSEPARATOR);
        strcat(pathname,name);
	first = 0;
    }
    nclistfree(path);
    return pathname;
}
