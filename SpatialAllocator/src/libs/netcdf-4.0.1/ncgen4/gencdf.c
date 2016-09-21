/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/gencdf.c,v 1.8 2009/03/11 18:26:17 dmh Exp $
 *********************************************************************/

#include "includes.h"
#include <ctype.h>	/* for isprint() */
#include "offsets.h"

extern List* vlendata;

/* Forward*/
static void check_err(const int stat, const int line, const char* file);
static void cdfloadattribute(Symbol* asym,Bytebuffer*);
static void cdfloadvar(Symbol* vsym,Bytebuffer*);
#ifdef USE_NETCDF4
static void gencdf_deftype(Symbol* tsym);
static void bindefinespecialattributes(Symbol* var);
#endif

/*
 * Generate C code for creating netCDF from in-memory structure.
 */
void
gen_netcdf(const char *filename)
{
    int stat, ncid;
    int idim, ivar, ityp, iatt;
    int ndims, nvars, natts, ngatts, ntyps, ngrps;

#ifdef USE_NETCDF4
    int igrp;
#endif

    Bytebuffer* databuf = bbNew();

    ndims = listlength(dimdefs);
    nvars = listlength(vardefs);
    natts = listlength(attdefs);
    ngatts = listlength(gattdefs);
    ntyps = listlength(typdefs);
    ngrps = listlength(grpdefs);

    /* create netCDF file, uses NC_CLOBBER mode */
    cmode_modifier |= NC_CLOBBER;
#ifdef USE_NETCDF4
    cmode_modifier |= NC_NETCDF4;
#endif

    stat = nc_create(filename, cmode_modifier, &ncid);
    check_err(stat,__LINE__,__FILE__);
    
    /* ncid created above is also root group*/
    rootgroup->ncid = ncid;

#ifdef USE_NETCDF4
    /* Define the group structure */
    /* walking grdefs list will do a preorder walk of all defined groups*/
    for(igrp=0;igrp<ngrps;igrp++) {
	Symbol* gsym = (Symbol*)listget(grpdefs,igrp);
	if(gsym == rootgroup) continue; /* ignore root group*/
	stat = nc_def_grp(gsym->container->ncid,gsym->name,&gsym->ncid);
	check_err(stat,__LINE__,__FILE__);
    }
#endif

#ifdef USE_NETCDF4
    /* Define the types*/
    if (ntyps > 0) {
	for(ityp = 0; ityp < ntyps; ityp++) {
	    Symbol* tsym = (Symbol*)listget(typdefs,ityp);
	    gencdf_deftype(tsym);
	}
    }
#endif

    /* define dimensions from info in dims array */
    if (ndims > 0) {
        for(idim = 0; idim < ndims; idim++) {
            Symbol* dsym = (Symbol*)listget(dimdefs,idim);
	    stat = nc_def_dim(dsym->container->ncid,
			      dsym->name,
			      dsym->dim.size,
			      &dsym->ncid);
	    check_err(stat,__LINE__,__FILE__);
       }
    }

    /* define variables from info in vars array */
    if (nvars > 0) {
	for(ivar = 0; ivar < nvars; ivar++) {
            Symbol* vsym = (Symbol*)listget(vardefs,ivar);
	    if (vsym->typ.dimset.ndims > 0) {	/* a dimensioned variable */
		/* construct a vector of dimension ids*/
		int dimids[NC_MAX_DIMS];
		for(idim=0;idim<vsym->typ.dimset.ndims;idim++)
		    dimids[idim] = vsym->typ.dimset.dimsyms[idim]->ncid;
		stat = nc_def_var(vsym->container->ncid,
				  vsym->name,
			          vsym->typ.basetype->ncid,
		        	  vsym->typ.dimset.ndims,
				  dimids,
				  &vsym->ncid);
	    } else { /* a scalar */
		stat = nc_def_var(vsym->container->ncid,
				  vsym->name,
			          vsym->typ.basetype->ncid,
		        	  vsym->typ.dimset.ndims,
				  NULL,
				  &vsym->ncid);
	    }
	    check_err(stat,__LINE__,__FILE__);
	}
    }

#ifdef USE_NETCDF4
    /* Collect vlen data*/
    gencdf_vlenconstants();

    /* define special variable properties */
    if(!usingclassic && nvars > 0) {
	for(ivar = 0; ivar < nvars; ivar++) {
            Symbol* var = (Symbol*)listget(vardefs,ivar);
	    bindefinespecialattributes(var);
	}
    }
#endif /*USE_NETCDF4*/

/* define global attributes */
    if(ngatts > 0) {
	for(iatt = 0; iatt < ngatts; iatt++) {
	    Symbol* gasym = (Symbol*)listget(gattdefs,iatt);
	    bbClear(databuf);
	    cdfloadattribute(gasym,databuf);	    
	}
    }
    
    /* define per-variable attributes */
    if(natts > 0) {
	for(iatt = 0; iatt < natts; iatt++) {
	    Symbol* asym = (Symbol*)listget(attdefs,iatt);
	    bbClear(databuf);
	    cdfloadattribute(asym,databuf);
	}
    }

    if (nofill_flag) {
	stat = nc_set_fill(rootgroup->ncid, NC_NOFILL, 0);
	check_err(stat,__LINE__,__FILE__);
    }

    /* leave define mode */
    stat = nc_enddef(rootgroup->ncid);
    check_err(stat,__LINE__,__FILE__);

    /* Load values into those variables with defined data */
    if(nvars > 0) {
        for(ivar = 0; ivar < nvars; ivar++) {
	    Symbol* vsym = (Symbol*)listget(vardefs,ivar);
	    if(vsym->data != NULL) {
	        bbClear(databuf);
		cdfloadvar(vsym,databuf);
	    }
	}
    }
    bbFree(databuf);
}

#ifdef USE_NETCDF4
static void
bindefinespecialattributes(Symbol* var)
{
    int stat;
    Specialdata* special = &var->var.special;
    if(special->flags & _STORAGE_FLAG) {
        int storage = special->_Storage;
        size_t* chunks = special->_ChunkSizes;
        if(special->nchunks == 0 || chunks == NULL) chunks = NULL;
        stat = nc_def_var_chunking(var->container->ncid,
                                   var->ncid,
                                   (storage == NC_CONTIGUOUS?NC_CONTIGUOUS
                                                            :NC_CHUNKED),
                                   chunks);
        check_err(stat,__LINE__,__FILE__);
    }   
    if(special->flags & _FLETCHER32_FLAG) {
        stat = nc_def_var_fletcher32(var->container->ncid,
                                     var->ncid,
                                     special->_Fletcher32);
        check_err(stat,__LINE__,__FILE__);
    }
    if(special->flags & (_DEFLATE_FLAG | _SHUFFLE_FLAG)) {
        stat = nc_def_var_deflate(var->container->ncid,
                                  var->ncid,
                                  (special->_Shuffle == 1?1:0),
                                  (special->_DeflateLevel >= 0?1:0),
                                  (special->_DeflateLevel >= 0?special->_DeflateLevel
                                                              :0));
        check_err(stat,__LINE__,__FILE__);
    }   
    if(special->flags & _ENDIAN_FLAG) {
        stat = nc_def_var_endian(var->container->ncid,
                                 var->ncid,
                                 (special->_Endianness == NC_ENDIAN_LITTLE?
                                        NC_ENDIAN_LITTLE
                                       :NC_ENDIAN_BIG));
        check_err(stat,__LINE__,__FILE__);
    }   
    if(special->flags & _NOFILL_FLAG) {
        stat = nc_def_var_fill(var->container->ncid,
                                 var->ncid,
		                 (special->_Fill?NC_FILL:NC_NOFILL),
                                 NULL);
        check_err(stat,__LINE__,__FILE__);
    }   
}
#endif /*USE_NETCDF4*/


void
cl_netcdf(void)
{
    int stat;
    stat = nc_close(rootgroup->ncid);
    check_err(stat,__LINE__,__FILE__);
}


#ifdef USE_NETCDF4
extern int H5Eprint1(FILE * stream);
#endif   

static void
check_err(const int stat, const int line, const char* file) {
    if (stat != NC_NOERR) {
	fprintf(stderr, "ncgen: %s\n", nc_strerror(stat));
	fprintf(stderr, "\t(%s:%d)\n", file,line);
#ifdef USE_NETCDF4
	H5Eprint1(stderr);
#endif   
	fflush(stderr);
	exit(1);
    }
}

#ifdef USE_NETCDF4
/*
Generate type definitions
*/
static void
gencdf_deftype(Symbol* tsym)
{
    int i,stat;

    assert(tsym->objectclass == NC_TYPE);
    switch (tsym->subclass) {
    case NC_PRIM: break; /* these are already taken care of*/
    case NC_OPAQUE:
	stat = nc_def_opaque(tsym->container->ncid,
			     tsym->typ.size,
			     tsym->name,
			     &tsym->ncid);
        check_err(stat,__LINE__,__FILE__);	
	break;
    case NC_ENUM: {
        Bytebuffer* datum;
        Datalist* ecdl;
	stat = nc_def_enum(tsym->container->ncid,
			   tsym->typ.basetype->typ.typecode,
			   tsym->name,
			   &tsym->ncid);
        check_err(stat,__LINE__,__FILE__);	
	datum = bbNew();
        ecdl = builddatalist(1);
        dlextend(ecdl); /* make room for one constant*/
	ecdl->length = 1;
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* econst = (Symbol*)listget(tsym->subnodes,i);
	    assert(econst->subclass == NC_ECONST);
	    ecdl->data[0] = econst->typ.econst;		
	    bbClear(datum);
	    gencdf_datalist(econst->typ.basetype,ecdl,datum,NULL);
	    stat = nc_insert_enum(tsym->container->ncid,
				  tsym->ncid,
				  econst->name,
				  bbContents(datum));
	    check_err(stat,__LINE__,__FILE__);	
	}
	bbFree(datum);
	ecdl->length = 0;
	} break;
    case NC_VLEN:
	stat = nc_def_vlen(tsym->container->ncid,
			   tsym->name,
			   tsym->typ.basetype->ncid,
			   &tsym->ncid);
        check_err(stat,__LINE__,__FILE__);	
	break;
    case NC_COMPOUND:
	stat = nc_def_compound(tsym->container->ncid,
			       tsym->typ.size,			       
			       tsym->name,
			       &tsym->ncid);			       
        check_err(stat,__LINE__,__FILE__);	
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* efield = (Symbol*)listget(tsym->subnodes,i);
	    assert(efield->subclass == NC_FIELD);
	    if(efield->typ.dimset.ndims == 0){ 
	        stat = nc_insert_compound(
				tsym->container->ncid,
				tsym->ncid,
				efield->name,
			        efield->typ.offset,
				efield->typ.basetype->ncid);
	    } else {
		int j;
		Bytebuffer* dimbuf = bbNew();
		/* Generate the field dimension constants*/
		for(j=0;j<efield->typ.dimset.ndims;j++) {
		     unsigned int size = efield->typ.dimset.dimsyms[j]->dim.size;
		     bbAppendn(dimbuf,(char*)&size,sizeof(size));
		}
	        stat = nc_insert_array_compound(
				tsym->container->ncid,
				tsym->ncid,
				efield->name,
			        efield->typ.offset,
				efield->typ.basetype->ncid,
				efield->typ.dimset.ndims,
				(int*)bbContents(dimbuf));
		bbFree(dimbuf);
	    }
            check_err(stat,__LINE__,__FILE__);	
	}
	break;
    default: panic("definectype: unexpected type subclass");
    }
}
#endif /*USE_NETCDF4*/

static void
cdfloadattribute(Symbol* asym,Bytebuffer* databuf)
{
    int stat;
    size_t len;
    Datalist* list;
    int varid, grpid, typid;
    Symbol* basetype = asym->typ.basetype;

    bbClear(databuf);

    grpid = asym->container->ncid,
    varid = (asym->att.var == NULL?NC_GLOBAL : asym->att.var->ncid);
    typid = basetype->ncid;

    list = asym->data;
    if(list == NULL) PANIC("empty attribute list");
    len = asym->att.count;

    /* Use the specialized put_att_XX routines if possible*/
    if(isprim(basetype->typ.typecode)) {
	if(basetype->typ.typecode == NC_STRING) {
	    const char** data;
    	    gencdf_datalist(asym,list,databuf,NULL);
	    data = (const char**)bbContents(databuf);
#ifdef USE_NETCDF4
            stat = nc_put_att_string(grpid,varid,asym->name,
				     bbLength(databuf)/sizeof(char*),
				     data);
#endif
	} else {
    	    gencdf_datalist(asym,list,databuf,NULL);
	    switch (basetype->typ.typecode) {
            case NC_BYTE: {
                signed char* data = (signed char*)bbContents(databuf);
                stat = nc_put_att_schar(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_CHAR: {
                char* data = (char*)bbContents(databuf);
		size_t slen = bbLength(databuf);
                stat = nc_put_att_text(grpid,varid,asym->name,slen,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_SHORT: {
                short* data = (short*)bbContents(databuf);
                stat = nc_put_att_short(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_INT: {
                int* data = (int*)bbContents(databuf);
                stat = nc_put_att_int(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_FLOAT: {
                float* data = (float*)bbContents(databuf);
                stat = nc_put_att_float(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_DOUBLE: {
                double* data = (double*)bbContents(databuf);
                stat = nc_put_att_double(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
#ifdef USE_NETCDF4
            case NC_UBYTE: {
                unsigned char* data = (unsigned char*)bbContents(databuf);
                stat = nc_put_att_ubyte(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_USHORT: {
                unsigned short* data = (unsigned short*)bbContents(databuf);
                stat = nc_put_att_ushort(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_UINT: {
                unsigned int* data = (unsigned int*)bbContents(databuf);
                stat = nc_put_att_uint(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_INT64: {
                long long* data = (long long*)bbContents(databuf);
                stat = nc_put_att_longlong(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
            case NC_UINT64: {
                unsigned long long* data = (unsigned long long*)bbContents(databuf);
                stat = nc_put_att_ulonglong(grpid,varid,asym->name,typid,len,data);
                check_err(stat,__LINE__,__FILE__);  
            } break;
#endif            
            default: PANIC1("cdfloadattribute: unexpected basetype: %d",basetype->typ.typecode);
            }
	}
    } else { /* use the generic put_attribute for user defined types*/
	const char* data;
	char out[4096];
    	gencdf_datalist(asym,list,databuf,NULL);
	data = (const char*)bbContents(databuf);
        stat = nc_put_att(grpid,varid,asym->name,typid,
			        len,(void*)data);
        check_err(stat,__LINE__,__FILE__);
	memset(out,0x77,sizeof(out));
	stat = nc_get_att(grpid,varid,asym->name,&out);
        check_err(stat,__LINE__,__FILE__);
    }
}

/* Define the put_vara closure function for cdf data*/
/*
Important assumptions:
1. The leftmost changed index controls the count set.
2. All indices to the right of #1 are assumed to be at there
   max values.
*/

static int
cdfputvara(struct Putvar* closure, Odometer* odom, Bytebuffer* data)
{
    int i;
    int stat = NC_NOERR;
    size_t startset[NC_MAX_DIMS];
    size_t countset[NC_MAX_DIMS];

#ifdef F
{
char vname[1024];
nc_inq_varname(closure->cdf.grpid,closure->cdf.varid,vname);
fprintf(stderr,"putvara: %s: ",vname);
fprintf(stderr,"|buffer|=%d\n",bbLength(data));
fprintf(stderr,"odom = %s\n",odometerprint(odom));
fprintf(stderr,"initial startset = [");
for(i=0;i<closure->rank;i++) fprintf(stderr,"%s%u",(i>0?", ":""),closure->cdf.startset[i]);
fprintf(stderr,"]\n");
fflush(stderr);
}
#endif

    /* Compute base on a change in the 0th dimension*/
    for(i=1;i<closure->rank;i++) {
        startset[i] = 0;
        countset[i] = odom->dims[i].index;
    }
    startset[0] = closure->startset[0];
    countset[0] = odom->dims[0].index - startset[0];

#ifdef F
{
    fprintf(stderr,"startset = [");
    for(i=0;i<closure->rank;i++) fprintf(stderr,"%s%u",(i>0?", ":""),startset[i]);
    fprintf(stderr,"] ");
    fprintf(stderr,"countset = [");
    for(i=0;i<closure->rank;i++) fprintf(stderr,"%s%u",(i>0?", ":""),countset[i]);
    fprintf(stderr,"]\n");
    fflush(stderr);
}
#endif

    stat = nc_put_vara(closure->cdf.grpid, closure->cdf.varid,
                       startset, countset,
                       bbContents(data));
    check_err(stat,__LINE__,__FILE__);

    for(i=0;i<closure->rank;i++) {
        closure->startset[i] = startset[i] + countset[i];
    }
done:
    bbClear(data);
    return stat;
}

static void
cdfloadvar(Symbol* vsym, Bytebuffer* databuf)
{
    int stat;
    int varid, grpid;
    int rank;
    void* data;
    Dimset* dimset = &vsym->typ.dimset;
    int isscalar = (dimset->ndims == 0);
    Putvar closure;

    grpid = vsym->container->ncid,
    varid = vsym->ncid;
    rank = vsym->typ.dimset.ndims;

    if(isscalar) { /* special handling*/
        gencdf_datalist(vsym,vsym->data,databuf,NULL);
        data = (void*)bbContents(databuf);
#ifdef USE_NETCDF4
        stat = nc_put_var(grpid, varid, data);
#else
	{
	size_t zero = 0;
        stat = nc_put_var1(grpid, varid, &zero, data);	
	}
#endif
        check_err(stat,__LINE__,__FILE__);
	return;
    }

    closure.putvar = cdfputvara;
    closure.rank = rank;
    closure.cdf.grpid = grpid;
    closure.cdf.varid = varid;
    memset(closure.startset,0,sizeof(closure.startset));
    bbClear(databuf);
    /* give the databuf a running start to be large enough*/
    if(vsym->typ.basetype->subclass == NC_PRIM) {
	size_t estimate = nctypesize(vsym->typ.basetype->typ.typecode)
			  * vsym->data->length;
	if(bbAlloc(databuf) < estimate)
	    bbSetalloc(databuf,estimate);
    }
    gencdf_datalist(vsym,vsym->data,databuf,&closure);
}

