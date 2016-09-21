/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdump.c,v 1.4 2009/03/30 15:47:25 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"
#include "dapdump.h"

#define CHECK(n) if((n) != NC_NOERR) {return (n);} else {}

int
dumpmetadata(int ncid, NChdr** hdrp)
{
    int stat,i,j,k;
    NChdr* hdr = (NChdr*)emalloc(sizeof(NChdr));
    MEMCHECK(hdr,NC_ENOMEM);
    memset((void*)hdr,0,sizeof(NChdr));
    hdr->ncid = ncid;
    hdr->content = ncbytesnew();
    if(hdrp) *hdrp = hdr;

    stat = nc_inq(hdr->ncid,
		  &hdr->ndims,
		  &hdr->nvars,
		  &hdr->ngatts,
		  &hdr->unlimid);
    CHECK(stat);
    if(ncdap3debug > 0) {
        fprintf(stdout,"ncid=%d ngatts=%d ndims=%d nvars=%d unlimid=%d\n",
		hdr->ncid,hdr->ngatts,hdr->ndims,hdr->nvars,hdr->unlimid);
    }
    hdr->gatts = (Attribute*)emalloc(hdr->ngatts*sizeof(Attribute));
    MEMCHECK(hdr->gatts,NC_ENOMEM);
    if(hdr->ngatts > 0)
	fprintf(stdout,"global attributes:\n");
    for(i=0;i<hdr->ngatts;i++) {
	Attribute* att = &hdr->gatts[i];
        char attname[NC_MAX_NAME];
	nc_type nctype;
	size_t typesize;
        stat = nc_inq_attname(hdr->ncid,NC_GLOBAL,i,attname);
        CHECK(stat);
	att->name = strdup(attname);
	stat = nc_inq_att(hdr->ncid,NC_GLOBAL,att->name,&nctype,&att->nvalues);
        CHECK(stat);
	att->etype = nctypetodap(nctype);
	fprintf(stdout,"\t[%d]: name=%s type=%s values(%lu)=",
			i,att->name,nctypetostring(octypetonc(att->etype)),(unsigned long)att->nvalues);
	typesize = octypesize(att->etype);
	if(nctype == NC_CHAR) {
	    size_t len = typesize*att->nvalues;
	    att->values = emalloc(len+1);/* for null terminate*/
	    MEMCHECK(att->values,NC_ENOMEM);
	    stat = nc_get_att(hdr->ncid,NC_GLOBAL,att->name,att->values);
            CHECK(stat);
	    att->values[len] = '\0';
	    fprintf(stdout," '%s'",att->values);
	} else {
	    size_t len = typesize*att->nvalues;
	    att->values = emalloc(len);
	    MEMCHECK(att->values,NC_ENOMEM);
	    stat = nc_get_att(hdr->ncid,NC_GLOBAL,att->name,att->values);
            CHECK(stat);
	    for(k=0;k<att->nvalues;k++) {
		fprintf(stdout," ");
		dumpdata1(octypetonc(att->etype),k,att->values);
	    }
	}
	fprintf(stdout,"\n");
    }

    hdr->dims = (Dim*)emalloc(hdr->ndims*sizeof(Dim));
    MEMCHECK(hdr->dims,NC_ENOMEM);
    for(i=0;i<hdr->ndims;i++) {
	hdr->dims[i].dimid = i;
        stat = nc_inq_dim(hdr->ncid,
	                  hdr->dims[i].dimid,
	                  hdr->dims[i].name,
	                  &hdr->dims[i].size);
        CHECK(stat);
	fprintf(stdout,"dim[%d]: name=%s size=%lu\n",
		i,hdr->dims[i].name,(unsigned long)hdr->dims[i].size);
    }    
    hdr->vars = (Var*)emalloc(hdr->nvars*sizeof(Var));
    MEMCHECK(hdr->vars,NC_ENOMEM);
    for(i=0;i<hdr->nvars;i++) {
	Var* var = &hdr->vars[i];
	nc_type nctype;
	var->varid = i;
        stat = nc_inq_var(hdr->ncid,
	                  var->varid,
	                  var->name,
			  &nctype,
			  &var->ndims,
			  var->dimids,
	                  &var->natts);
        CHECK(stat);
	var->nctype = (nctype);
	fprintf(stdout,"var[%d]: name=%s type=%s |dims|=%d",
		i,
		var->name,
		nctypetostring(var->nctype),
		var->ndims);
	fprintf(stdout," dims={");
	for(j=0;j<var->ndims;j++) {
	    fprintf(stdout," %d",var->dimids[j]);
	}
	fprintf(stdout,"}\n");
	var->atts = (Attribute*)emalloc(var->natts*sizeof(Attribute));
        MEMCHECK(var->atts,NC_ENOMEM);
        for(j=0;j<var->natts;j++) {
	    Attribute* att = &var->atts[j];
	    char attname[NC_MAX_NAME];
	    size_t typesize;
	    nc_type nctype;
            stat = nc_inq_attname(hdr->ncid,var->varid,j,attname);
	    CHECK(stat);
	    att->name = strdup(attname);
	    stat = nc_inq_att(hdr->ncid,var->varid,att->name,&nctype,&att->nvalues);
	    CHECK(stat);
	    att->etype = nctypetodap(nctype);
	    typesize = octypesize(att->etype);
	    att->values = emalloc(typesize*att->nvalues);
	    MEMCHECK(att->values,NC_ENOMEM);
	    stat = nc_get_att(hdr->ncid,var->varid,att->name,att->values);
            CHECK(stat);
	    fprintf(stdout,"\tattr[%d]: name=%s type=%s values(%lu)=",
			j,att->name,nctypetostring(octypetonc(att->etype)),(unsigned long)att->nvalues);
	    for(k=0;k<att->nvalues;k++) {
		fprintf(stdout," ");
		dumpdata1(octypetonc(att->etype),k,att->values);
	    }
	    fprintf(stdout,"\n");
	}
    }    
    fflush(stdout);
    return NC_NOERR;
}

void
dumpdata1(nc_type nctype, size_t index, char* data)
{
    switch (nctype) {
    case NC_CHAR:
	fprintf(stdout,"'%c' %hhd",data[index],data[index]);
	break;
    case NC_BYTE:
	fprintf(stdout,"%hdB",((signed char*)data)[index]);
	break;
    case NC_UBYTE:
	fprintf(stdout,"%huB",((unsigned char*)data)[index]);
	break;
    case NC_SHORT:
	fprintf(stdout,"%hdS",((short*)data)[index]);
	break;
    case NC_USHORT:
	fprintf(stdout,"%hdUS",((unsigned short*)data)[index]);
	break;
    case NC_INT:
	fprintf(stdout,"%d",((int*)data)[index]);
	break;
    case NC_UINT:
	fprintf(stdout,"%uU",((unsigned int*)data)[index]);
	break;
    case NC_FLOAT:
	fprintf(stdout,"%#gF",((float*)data)[index]);
	break;
    case NC_DOUBLE:
	fprintf(stdout,"%#gD",((double*)data)[index]);
	break;
    case NC_STRING:
	fprintf(stdout,"\"%s\"",((char**)data)[index]);
	break;
    default:
	fprintf(stdout,"Unknown type: %i",nctype);
	break;
    }
    fflush(stdout);
}
