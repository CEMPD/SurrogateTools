/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/getvar3.c,v 1.2 2009/03/30 15:47:25 dmh Exp $
 *********************************************************************/
#include "ncdap3.h"
#include "dapodom.h"
#include "getvar3.h"

/* Forward:*/
static void buildvarprojection(NCDRNO*,Getvar* getvar, NClist* path, NCbytebuffer* buf);
static void makevarprojection(NCslice* slices, int dimdex, int ndims, NCbytebuffer* buf);
static NCerror validategetvar(NCDRNO*, Getvar* getvar);
static NCerror correlate(CDFnode*, OCnode*);
static Getvar* makegetvar(NCDRNO*, CDFnode* var,
			  const size_t* startp,const size_t* countp, const ptrdiff_t* stridep,
			  void* data, nc_type dsttype);

static NCerror moveto(NCDRNO*, Getvar*, CDFnode* dataroot, void* memory);
static NCerror moveto1(NCDRNO*, OCcontent* currentcontent,
		   NClist* path, int depth,
		   Getvar*, int dimindex,
		   struct NCMEMORY*);

static size_t findfield(CDFnode* node, CDFnode* subnode);
static NCerror slicestring(OCconnection*, OCcontent*, NCslice*, struct NCMEMORY*);
static int iswholevariable(Dapodometer* odom);

NCerror
nc3d_getvarx(int ncid, int varid,
	    const size_t *startp,
	    const size_t *countp,
	    const ptrdiff_t* stridep,
	    void *value,
	    nc_type dsttype0)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    int i;
    NC* ncp;
    NCDRNO* drno;
    NC_var* var;
    CDFnode* cdfvar; /* cdf node mapping to var*/
    CDFnode* cachevar; /* cdf node from previous call */
    NClist* ddsnodes;
    nc_type dsttype;

    ncstat = NC_check_id(ncid, &ncp); 
    if(ncstat != NC_NOERR) goto fail;

    drno = ncp->drno;
    
    var = NC_lookupvar(ncp,varid);
    if(var == NULL) {ncstat = NC_ENOTVAR; goto fail;}

    /* Find cdfnode corresponding to the var.*/
    ddsnodes = drno->cdf.cdfnodes;
    cdfvar = NULL;
    for(i=0;i<nclistlength(ddsnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(ddsnodes,i);
	if(node->nctype == NC_Primitive && node->ncid == varid) {
	    cdfvar = node;
	    break;
	}
    }
    ASSERT((cdfvar != NULL));
    ASSERT((strcmp(cdfvar->ncfullname,var->name->cp)==0));

    dsttype = (dsttype0);

    /* Default to using the inquiry type for this var*/
    if(dsttype == NC_NAT) dsttype = cdfvar->externaltype;

    /* Validate any implied type conversion*/
    if(cdfvar->etype != dsttype) {
	/* The only disallowed conversion is to/from char and numeric types*/
	/* String|URL->char is allowed*/
	if(cdfvar->etype != NC_STRING && cdfvar->etype != NC_URL && dsttype == NC_CHAR)
	    return THROW(NC_ECHAR);
    }

    /* Remember variable last read */
    cachevar = (drno->var.varinfo == NULL?NULL:drno->var.varinfo->target);

    /* Construct a new getvar info */
    if(drno->var.varinfo != NULL) {
        efree(drno->var.varinfo);
	drno->var.varinfo=NULL;
    }
    drno->var.varinfo = makegetvar(drno,cdfvar,startp,countp,stridep,
                                       value,dsttype);
    ncstat = validategetvar(drno, drno->var.varinfo);
    if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}

#ifndef PARTIALVAR
    /* if the new variable is same as old, then do not re-read */
    if(cachevar != NULL && cachevar == cdfvar) {
	if(ncdap3debug > 0)
	    fprintf(stderr,"Using cached variable: %s\n",cachevar->name);
    } else
#endif
    {
        char* url;
        char* proj;
        char* sel;
        size_t plen;
        NCbytebuffer* newproj;
        NCbytebuffer* newurl;
	NClist* path;
        Getvar* getvar;
        getvar = drno->var.varinfo;
        /* Construct the projection to return just the requested variable. */
        /* Note the whole variable is returned (see buildvarprojection())*/
        url = drno->dap.url.url;
        proj = drno->dap.url.projection;
        sel = drno->dap.url.selection;
        newproj = ncbytesnew();
        if(proj) {ncbytescat(newproj,proj); ncbytescat(newproj,",");}
        plen = ncbyteslength(newproj);
        path = nclistnew();
        collectnodepath(cdfvar,path);
        buildvarprojection(drno,getvar,path,newproj);
        if(ncbyteslength(newproj) == plen) { /* no projection */
	    PANIC("getvar3: null projection");
	    /*ncbytessetlength(newproj,plen-1);*/ /* remove trailing comma*/
	}
        nclistfree(path);
        plen = ncbyteslength(newproj);
        newurl = ncbytesnew();
        ncbytescat(newurl,url);
        if(plen > 0) {ncbytescat(newurl,"?"); ncbytesappendn(newurl,ncbytescontents(newproj),plen);}
        if(sel) ncbytescat(newurl,sel);
        url = ncbytesdup(newurl); /* final url*/
        ncbytesfree(newproj);
        ncbytesfree(newurl);

        /* (re-)read the datadds*/
        oc_dataddsclear(drno->dap.conn);
        ocstat = oc_fetchdatadds(drno->dap.conn,url);
        free(url);
        if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
        /* Match datadds nodes to corresponding dds nodes*/
        ncstat = correlate(drno->cdf.root,oc_getdatadds(drno->dap.conn));
        if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
    }

    /* Now, walk to the relevant instance*/
    assert((cdfvar->datadds != NULL));
    ncstat = moveto(drno,drno->var.varinfo,drno->cdf.root,value);
    if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
    goto ok;
fail:
ok:
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

/* The datadds dds should be a subset of the dds and with*/
/* different dimension decl sizes. Walk the datadds and make*/
/* its nodes point to the corresponding dds node that*/
/* was created during ncd_open.*/
static NCerror
correlate(CDFnode* cdf, OCnode* dataddsnode)
{
    int i,j;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;

    if(cdf->nctype != octypetonc(dataddsnode->octype)) {ncstat = NC_EINVAL; {THROW(ncstat); goto fail;}}
    if(strcmp(dataddsnode->name,cdf->name) != 0) {ncstat = NC_EINVAL; {THROW(ncstat); goto fail;}}
    cdf->datadds = dataddsnode;
    switch (cdf->nctype) {
    case NC_Dataset:
    case NC_Structure:
    case NC_Grid:
    case NC_Sequence:
	for(i=0;i<oclistlength(dataddsnode->subnodes);i++) {
	    OCnode* datadds = (OCnode*)oclistget(dataddsnode->subnodes,i);
	    for(j=0;j<nclistlength(cdf->subnodes);j++) {
		CDFnode* subnode = (CDFnode*)nclistget(cdf->subnodes,j);
		if(strcmp(datadds->name,subnode->name) == 0) {
		    ncstat = correlate(subnode,datadds);
		    if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
		    break;
		}
	    }
	}
	break;

    case NC_Primitive:
	break;
	
    default: PANIC1("unexpected node type: %d",cdf->nctype)
    }
fail:
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static NCerror
validategetvar(NCDRNO* drno, Getvar* getvar)
{
    int i;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    CDFnode* var = getvar->target;
    int rank;
    NClist* dimset = nclistnew();
    NClist* path = nclistnew();
    NCslice* slice;

    collectnodepath(var,path);
    collectdims(drno,path,dimset);
    rank = nclistlength(dimset);

    slice = getvar->slices;
    for(i=0;i<rank;i++,slice++) {
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
	size_t size = dim->dim.declsize;
	ASSERT((size > 0));
	slice->declsize = size;
	if(slice->first >= size) {ncstat=NC_EINVALCOORDS; {THROW(ncstat); goto fail;}}
	if(slice->first + slice->count > size) {ncstat=NC_EEDGE; {THROW(ncstat); goto fail;}}
	if(slice->stride == 0) {ncstat=NC_ESTRIDE; {THROW(ncstat); goto fail;}}
    }
fail:
    nclistfree(dimset);
    nclistfree(path);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static Getvar*
makegetvar(NCDRNO* drno, CDFnode* var,
	   const size_t* startp, const size_t* countp, const ptrdiff_t* stridep,
	   void* data, nc_type dsttype)
{
    int i;
    int rank;
    NClist* dimset;
    NClist* path;
    Getvar* getvar;

    getvar = (Getvar*)emalloc(sizeof(Getvar));
    MEMCHECK(getvar,(Getvar*)NULL);
    dimset = nclistnew();
    path = nclistnew();

    collectnodepath(var,path);
    collectdims(drno,path,dimset);
    rank = nclistlength(dimset);

    getvar->target = var;
    getvar->nslices = rank;
    
    for(i=0;i<rank;i++) {
	getvar->slices[i].first	 = startp[i];
	getvar->slices[i].count	 = countp[i];
	getvar->slices[i].stride = stridep[i];
	getvar->slices[i].stop	 = getvar->slices[i].first+getvar->slices[i].count;
    }
    getvar->memory = data;
    getvar->dsttype = dsttype;
    getvar->target = var;
    nclistfree(dimset);
    nclistfree(path);
    return getvar;
}


/* In order to construct the projection,*/
/* we need to make sure to match the relevant dimensions*/
/* against the relevant nodes in which the ultimate target*/
/* is contained. Must take simulated unlimited*/
/* and string pseudo-dimension into account.*/
/* Currently only whole variables are read; PARTIALVAR*/
/* turns off code to apply slices as projections*/
static void
buildvarprojection(NCDRNO* drno, Getvar* getvar, NClist* path, NCbytebuffer* buf)
{
    int i, dimdex, pathlen, first;
    int hasunlimdim, hasstringdim;
    CDFnode* node;

    ncbytesclear(buf);
    ncbytesclear(buf);
    hasunlimdim = (drno->cdf.sequnlimited != NULL);

    hasstringdim = (getvar->target->nctype == NC_Primitive 
			&& (getvar->target->etype == NC_STRING
			    || getvar->target->etype == NC_URL));

    pathlen = nclistlength(path);

    for(first=1,dimdex=0,i=0;i<pathlen;i++) {
#ifdef PARTIALVAR
	NCslice* slice = &getvar->slices[dimdex];
#endif
	node = (CDFnode*)nclistget(path,i);
	if(node->nctype == NC_Dataset) continue;
	if(first) {first=0;} else {ncbytescat(buf,".");} /* we are past the first projection*/
	ncbytescat(buf,node->name);
	if(node->array.rank == 0 && node != drno->cdf.sequnlimited) continue;
	/* If the slice is the full variable, then elide the projection part*/
	if(node == drno->cdf.sequnlimited) {
	    /* Project the sequence in the hope that the server will pay attention to it*/
	    makevarprojection(getvar->slices,dimdex,node->array.rank,buf);
	    dimdex++;
	} else {
#ifdef PARTIALVAR
/* WARNING: do not turn this flag on until moveto1 has been*/
/* fixed correspondingly	*/
	    if(slice->first != 0 || slice->stride != 1 || slice->stop != slice->declsize) {
	        if(i == (pathlen - 1) && hasstringdim) {
		    makevarprojection(getvar->slices,dimdex,node->array.rank-1,buf);
	        } else {
		    makevarprojection(getvar->slices,dimdex,node->array.rank,buf);
	        }
	    }
#endif
	    dimdex += node->array.rank;
	}
    }
}

static void
makevarprojection(NCslice* slices, int dimdex, int ndims, NCbytebuffer* buf)
{
    int i;
    char tmp[1024];
    for(i=0;i<ndims;i++) {
	NCslice* slice = slices+(i+dimdex);
	snprintf(tmp,sizeof(tmp),"[%lu:%lu:%lu]",
			(unsigned long)slice->first,
			(unsigned long)slice->stride,
			(unsigned long)(slice->stop - 1));
	ncbytescat(buf,tmp);
    }
}

/* Warning: the target node owner is the datadds, not the dds*/
/* as is the datanode; Distinquish the source of a node*/
/* by appending 0 to variable names whose node value comes from the dds.*/
static NCerror
moveto(NCDRNO* drno, Getvar* getvar, CDFnode* rootnode, void* memory)
{
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    OCconnection* conn = drno->dap.conn;
    OCcontent* rootcontent;
    NClist* path = nclistnew();
    struct NCMEMORY memstate;

    memstate.next = (memstate.memory = memory);

    /* Get the root content*/
    rootcontent = oc_newcontent(conn);
    ocstat = oc_rootcontent(conn,rootcontent);
    if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}

    collectnodepath(getvar->target,path); /* path is in datadds*/

    ncstat = moveto1(drno,rootcontent,path,0,getvar,0,&memstate);
fail:
    nclistfree(path);
    oc_freecontent(conn,rootcontent);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static NCerror
moveto1(NCDRNO* drno,
	OCcontent* currentcontent,
	NClist* path, int depth, /* depth is position in path*/
	Getvar* getvar, int dimindex, /* dimindex is position in getvar->slices*/
	struct NCMEMORY* memory)
{
    int i;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    size_t fieldindex,rank;
    OCconnection* conn = drno->dap.conn;
    CDFnode* thisnode = (CDFnode*)nclistget(path,depth);
    OCcontent* reccontent;
    OCcontent* dimcontent;
    OCcontent* fieldcontent;
    OCcontent* stringcontent;
    Dapodometer* odom;
    OCmode mode;

    fieldcontent = oc_newcontent(conn);
    dimcontent = oc_newcontent(conn);
    reccontent = oc_newcontent(conn);
    stringcontent = oc_newcontent(conn);

    switch (thisnode->nctype) {
    case NC_Dataset:
    case NC_Grid:
	/* Since these are never dimensioned, we can go directly to*/
	/* the appropriate field; locate the field index for the next*/
	/* item in the path*/
	ASSERT((((CDFnode*)nclistget(path,depth+1)) != NULL));
	fieldindex = findfield(thisnode,(CDFnode*)nclistget(path,depth+1));
	ocstat = oc_fieldcontent(conn,currentcontent,fieldcontent,fieldindex);
	if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
	ncstat = moveto1(drno,fieldcontent,path,depth+1,getvar,dimindex,memory);
	break;

    case NC_Structure:
	/* First, we need to go thru the index, then the field*/
	/* Check for scalar vs dimensioned*/
	mode = oc_contentmode(conn,currentcontent);
	if(mode == Dimmode) {
	    /* figure out which slices refer to this node: dimindex upto dimindex+rank*/
	    size_t dimoffset;
	    rank = thisnode->array.rank;
	    ASSERT(((dimindex+rank <= getvar->nslices)));
	    odom = newdapodometer(getvar->slices,dimindex,rank);
	    while(dapodometermore(odom)) {
	        /* Compute which instance to move to*/
	        dimoffset = dapodometercount(odom);
	        ocstat = oc_dimcontent(conn,currentcontent,dimcontent,dimoffset);
	        if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
	        ASSERT((oc_contentmode(conn,dimcontent)==Fieldmode));
	        ncstat = moveto1(drno,dimcontent,
				 path,depth,
				 getvar,dimindex+rank,
				 memory);
		dapodometerincr(odom);
	    }
	    freedapodometer(odom);
	} else if(mode == Fieldmode) { /* move directly to the field of this instance*/
	    fieldindex = findfield(thisnode,(CDFnode*)nclistget(path,depth+1));
	    ocstat = oc_fieldcontent(conn,currentcontent,fieldcontent,fieldindex);
	    if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
	    ncstat = moveto1(drno,fieldcontent,
			     path,depth+1,
			     getvar,dimindex,
			      memory);
	} else PANIC1("moveto: unexpected structure content mode: %d",(int)mode);
	break;

    case NC_Primitive: /* Copy out all of the data into the appropriate point in memory*/
	/* Handle vars with string pseudo-dimension separately*/
	if(thisnode->etype == NC_STRING || thisnode->etype == NC_URL) {
	    NCslice stringslice = getvar->slices[getvar->nslices-1]; /* slice for string pseudo-dimension*/
	    if(thisnode->array.rank == 1) { /* really a scalar string*/
	        ASSERT((oc_contentmode(conn,currentcontent) == Datamode));
		ncstat = slicestring(conn,currentcontent,&stringslice,memory);
		if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
	    } else { /* rank > 1*/
		/* Walk to each string individually and extract the relevant subset*/
	        Dapodometer* odom = newdapodometer(getvar->slices,dimindex,thisnode->array.rank-1);
		while(dapodometermore(odom)) {
		    size_t dimoffset = dapodometercount(odom);
		    ocstat = oc_dimcontent(conn,currentcontent,stringcontent,dimoffset);
		    if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		    ncstat = slicestring(conn,stringcontent,&stringslice,memory);
		    if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
		    dapodometerincr(odom);
		}
                freedapodometer(odom);
	    }
	} else { /* non-string primitive*/
	    size_t memtypesize = nctypesizeof(getvar->dsttype);
	    if(oc_contentmode(conn,currentcontent) == Datamode) { /* singleton data item*/
	        char value[16]; /* to hold any value*/
		ocstat = oc_getcontent(conn,currentcontent,value,sizeof(value),0,1);
		if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		ncstat = ncdap3convert(thisnode->etype,getvar->dsttype,memory->next,value);
		if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
		memory->next += memtypesize;
	    } else if(oc_contentmode(conn,currentcontent) == Dimmode) { /* array of data*/
	        Dapodometer* odom = newdapodometer(getvar->slices,dimindex,thisnode->array.rank);
		/* Optimize off the use of the odometer by checking the slicing*/
                /* to see if the whole variable is being extracted.*/
		/* However do not do this if the external type conversion is needed*/
		if(iswholevariable(odom) && (thisnode->etype == getvar->dsttype)) {
		    /* Read the whole n elements directly into memory.*/
		    /* Validate the count*/
		    size_t odomsize = dapodometertotal(odom);		
		    ocstat = oc_getcontent(conn,currentcontent,memory->next,odomsize*memtypesize,0,odomsize);
	            if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		    memory->next += (memtypesize*odomsize);
		} else { /* Oh well, use the odometer to walk to the appropriate fields*/
	            char value[16]; /* to hold any value*/
		    while(dapodometermore(odom)) {
		        size_t dimoffset = dapodometercount(odom);
		        ocstat = oc_getcontent(conn,currentcontent,value,sizeof(value),dimoffset,1);
		        if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		        ncstat = ncdap3convert(thisnode->etype,getvar->dsttype,memory->next,value);
		        if(ncstat != NC_NOERR) {THROW(ncstat); goto fail;}
		        memory->next += memtypesize;
			dapodometerincr(odom);
	            }
		}
                freedapodometer(odom);
	    } else PANIC("illegal mode");
	}
	break;
    
    case NC_Sequence:
	if(thisnode != drno->cdf.sequnlimited) {
	    ncstat = OC_EINVAL;
	    {THROW(ncstat); goto fail;}
	} else { /* This is a simulated unlimited sequence node */
	    NCslice* uslice;
	    ASSERT((nclistlength(path) >= 3)); /* at least dataset,sequence,target*/
	    ASSERT((oc_contentmode(conn,currentcontent) == Recordmode));
	    /* use uslice to walk the sequence*/
	    uslice = &getvar->slices[0];
	    for(i=uslice->first;i<uslice->stop;i+=uslice->stride) {
		ocstat = oc_recordcontent(conn,currentcontent,reccontent,i);
		if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		fieldindex = findfield(thisnode,(CDFnode*)nclistget(path,depth+1));
		ocstat = oc_fieldcontent(conn,reccontent,fieldcontent,fieldindex);
		if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
		ncstat = moveto1(drno,fieldcontent,path,depth+1,getvar,dimindex+1,memory);
	    }
	}
	break;

    default: ncstat = NC_EINVAL; {THROW(ncstat); goto fail;}
    }
    goto ok;
fail:
ok:
    oc_freecontent(conn,dimcontent);
    oc_freecontent(conn,fieldcontent);
    oc_freecontent(conn,reccontent);
    oc_freecontent(conn,stringcontent);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static size_t
findfield(CDFnode* node, CDFnode* subnode)
{
    size_t i;
    for(i=0;i<nclistlength(node->subnodes);i++) {
	CDFnode* test = (CDFnode*) nclistget(node->subnodes,i);
	if(test == subnode) return i;
    }
    return 0;
}

/* Extract a slice of a string; many special cases to consider and optimize*/
static NCerror
slicestring(OCconnection* conn, OCcontent* content, NCslice* slice, struct NCMEMORY* memory)
{
    char*  stringmem = NULL;
    size_t stringlen;
    unsigned int i;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;

    /* Get the whole string into local memory*/
    ocstat = oc_stringcontent(conn,content,&stringmem,&stringlen);
    if(ocstat != OC_NOERR) {THROW(ocstat); goto fail;}
    /* Step by stride across string; if we go past end of string, then pad*/
    for(i=slice->first;i<slice->count;i+=slice->stride) {
	if(i < stringlen)
	    *memory->next++ = stringmem[i];
	else /* i >= stringlen*/
	    *memory->next++ = NC_FILL_CHAR;
    }
fail:
    if(stringmem != NULL) efree(stringmem);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static int 
iswholevariable(Dapodometer* odom)
{
    unsigned int i;
    for(i=0;i<odom->rank;i++) {
	ASSERT((odom->slices[i].declsize != 0));
	if(odom->slices[i].first != 0 || odom->slices[i].stride != 1
	   || odom->slices[i].count != odom->slices[i].declsize)
	    return 0;
    }
    return 1;
}
