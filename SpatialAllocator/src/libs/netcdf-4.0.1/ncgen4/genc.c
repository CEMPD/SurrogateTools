/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/genc.c,v 1.8 2009/03/11 18:26:17 dmh Exp $
 *********************************************************************/

#include "includes.h"
#include <ctype.h>	/* for isprint() */
#include "bytebuffer.h"

/* Forward */
static const char* ncstype(nc_type);
static const char* nctype(nc_type);
static const char* ncctype(nc_type);
static const char* groupncid(Symbol*);
static const char* typencid(Symbol*);
static const char* varncid(Symbol*);
static const char* dimncid(Symbol*);

#ifdef USE_NETCDF4
static void definectype(Symbol*);
static char* cprefixed(List* prefix, char* suffix, char* separator);
static void genc_deftype(Symbol*);
static void definespecialattributes(Symbol* vsym);
#endif

static void cloadattribute(Symbol* asym);
static void cloadvar(Symbol*);

static void computemaxunlimited(void);

/*
Global Bytebuffer into which to store the C code output;
periodically dumped to file by flushcode().
*/
Bytebuffer* ccode;
/* Fixed size buffer for short sprintf target */
char stmt[1024];

/*
 * Generate C code for creating netCDF from in-memory structure.
 */
void
gen_ncc(const char *filename)
{
    int idim, ivar, iatt, maxdims;
    int ndims, nvars, natts, ngatts, ngrps, ntyps;
    char* cmode_string;

#ifdef USE_NETCDF4
    int igrp,ityp;
#endif

    ccode = bbNew();
    bbSetalloc(ccode,C_MAX_STMT);

    ndims = listlength(dimdefs);
    nvars = listlength(vardefs);
    natts = listlength(attdefs);
    ngatts = listlength(gattdefs);
    ngrps = listlength(grpdefs);
    ntyps = listlength(typdefs);

    /* wrap in main program */
    cline("#include <stdio.h>");
    cline("#include <stdlib.h>");
    cline("#include <netcdf.h>");
    cline("");
    flushcode();

#ifdef USE_NETCDF4

    /* Construct C type definitions*/
    if (ntyps > 0) {
	for(ityp = 0; ityp < ntyps; ityp++) {
	    Symbol* tsym = (Symbol*)listget(typdefs,ityp);
	    definectype(tsym);
	}
	bbCat(ccode,"");
    }
    flushcode();

    /*
	Define vlen constants
	The idea is to walk all the data lists
	whose variable type has a vlen and collect
	the vlen data and define a constant for it.
    */ 
    {
	flushcode(); /* dump code to this point*/
	genc_vlenconstants(ccode);
	cline("");
    }

    /* Construct the chunking constants*/
    if(!usingclassic) {
        for(ivar=0;ivar<nvars;ivar++) {
            Bytebuffer* tmp = bbNew();
            Symbol* var = (Symbol*)listget(vardefs,ivar);
            Specialdata* special = &var->var.special;
            if(special->flags & _CHUNKSIZE_FLAG) {
                int i;
                size_t* chunks = special->_ChunkSizes;
                if(special->nchunks == 0 || chunks == NULL) continue;
                bbClear(tmp);
                for(i=0;i<special->nchunks;i++) {
                    nprintf(stmt,sizeof(stmt),"%s%ld",
                            (i == 0?"":", "),special->_ChunkSizes[i]);
                    bbCat(tmp,stmt);
                }
                nprintf(stmt,sizeof(stmt),"static size_t %s_chunksizes[%d] = {",
                            cname(var),special->nchunks);
                cpartial(stmt);
                cprint(tmp);
                cline("} ;");
            }
        }
	cline("");
    }
#endif /*USE_NETCDF4*/

    /* Now construct the main procedures*/
    cline("void");
    cline("check_err(const int stat, const int line, const char *file) {");
    clined(1,"if (stat != NC_NOERR) {");
    clined(2,"(void)fprintf(stderr,\"line %d of %s: %s\\n\", line, file, nc_strerror(stat));");
    clined(2,"fflush(stderr);");
    clined(2,"exit(1);");
    clined(1,"}");
    cline("}");
    cline("");
    cline("int");
    nprintf(stmt,sizeof(stmt), "%s() {/* create %s */", mainname, filename);
    cline(stmt);
    /* create necessary declarations */
    cline("");    
    clined(1,"int  stat;  /* return status */");
    clined(1,"int  ncid;  /* netCDF id */");
    flushcode();

#ifdef USE_NETCDF4
    /* Define variables to hold group ids*/
    if(!usingclassic) {
        cline("");
        clined(1,"/* group ids */");
    }
    if(!usingclassic && ngrps > 0) {    
        for(igrp = 0; igrp < ngrps; igrp++) {
	    Symbol* gsym = (Symbol*)listget(grpdefs,igrp);
	    nprintf(stmt,sizeof(stmt),"%sint %s;",indented(1),groupncid(gsym));
	    cline(stmt);
	}
    }

    /* define variables to hold type ids*/
    if(!usingclassic && ntyps > 0) {
	cline("");
	clined(1,"/* type ids */");
	for(ityp = 0; ityp < ntyps; ityp++) {
	    Symbol* tsym = (Symbol*)listget(typdefs,ityp);
	    nprintf(stmt,sizeof(stmt),"%sint %s;",indented(1), typencid(tsym));
	    cline(stmt);
	}
    }
    flushcode();
#endif

    if (ndims > 0) {
	cline("");
	clined(1,"/* dimension ids */");
	for(idim = 0; idim < ndims; idim++) {
	    Symbol* dsym = (Symbol*)listget(dimdefs,idim);
	    nprintf(stmt,sizeof(stmt),"%sint %s;", indented(1), dimncid(dsym));
	    cline(stmt);
	}

	cline("");
	clined(1,"/* dimension lengths */");
	for(idim = 0; idim < ndims; idim++) {
	    Symbol* dsym = (Symbol*)listget(dimdefs,idim);
	    if (dsym->dim.size == NC_UNLIMITED) {
		nprintf(stmt,sizeof(stmt),"%ssize_t %s_len = NC_UNLIMITED;",
			indented(1),cname(dsym));
	    } else {
		nprintf(stmt,sizeof(stmt),"%ssize_t %s_len = %lu;",
			indented(1),
			cname(dsym),
			(unsigned long) dsym->dim.size);
	    }
	    cline(stmt);
	}
    }
    flushcode();

    maxdims = 0;	/* most dimensions of any variable */
    for(ivar = 0; ivar < nvars; ivar++) {
      Symbol* vsym = (Symbol*)listget(vardefs,ivar);
      if(vsym->typ.dimset.ndims > maxdims)
	maxdims = vsym->typ.dimset.ndims;
    }

    if (nvars > 0) {
	cline("");
	clined(1,"/* variable ids */");
	for(ivar = 0; ivar < nvars; ivar++) {
            Symbol* vsym = (Symbol*)listget(vardefs,ivar);
	    nprintf(stmt,sizeof(stmt),"    int %s;", varncid(vsym));
	    cline(stmt);
	}

	cline("");
	clined(1,"/* rank (number of dimensions) for each variable */");
	for(ivar = 0; ivar < nvars; ivar++) {
            Symbol* vsym = (Symbol*)listget(vardefs,ivar);
	    nprintf(stmt,sizeof(stmt),"#   define RANK_%s %d", cname(vsym),
		    vsym->typ.dimset.ndims);
	    cline(stmt);
	}
	if (maxdims > 0) {	/* we have dimensioned variables */
	    cline("");
	    clined(1,"/* variable shapes */");
	    for(ivar = 0; ivar < nvars; ivar++) {
                Symbol* vsym = (Symbol*)listget(vardefs,ivar);
		if (vsym->typ.dimset.ndims > 0) {
		    nprintf(stmt,sizeof(stmt),"    int %s_dims[RANK_%s];",
			    cname(vsym), cname(vsym));
		    cline(stmt);
		}
	    }
	}
    }
    flushcode();

    /* create netCDF file, uses NC_CLOBBER mode */
    cline("");
    clined(1,"/* enter define mode */");

    if (!cmode_modifier) {
	cmode_string = "NC_CLOBBER";
    } else if (cmode_modifier & NC_64BIT_OFFSET) {
	cmode_string = "NC_CLOBBER|NC_64BIT_OFFSET";
#ifdef USE_NETCDF4
    } else if (cmode_modifier & NC_CLASSIC_MODEL) {
	cmode_string = "NC_CLOBBER|NC_NETCDF4|NC_CLASSIC_MODEL";
    } else if (cmode_modifier & NC_NETCDF4) {
	cmode_string = "NC_CLOBBER|NC_NETCDF4";	
#endif
    } else {
        derror("unknown cmode modifier");
	cmode_string = "NC_CLOBBER";
    }
    nprintf(stmt,sizeof(stmt),"    stat = nc_create(\"%s\", %s, &ncid);",
		 filename,cmode_string);
    cline(stmt);
    clined(1,"check_err(stat,__LINE__,__FILE__);");
    flushcode();
    
#ifdef USE_NETCDF4
    /* Define the group structure */
    /* ncid created above is also root group*/
    if(!usingclassic) {
        nprintf(stmt,sizeof(stmt),"    %s = ncid;",groupncid(rootgroup));
        cline(stmt);
        /* walking grpdefs list will do a preorder walk of all defined groups*/
        for(igrp=0;igrp<listlength(grpdefs);igrp++) {
	    Symbol* gsym = (Symbol*)listget(grpdefs,igrp);
	    if(gsym == rootgroup) continue; /* ignore root*/
	    if(gsym->container == NULL)
		PANIC("null container");
	    nprintf(stmt,sizeof(stmt),
		"    stat = nc_def_grp(%s, \"%s\", &%s);",
		groupncid(gsym->container),
		gsym->name, groupncid(gsym));
	    cline(stmt);
	    clined(1,"check_err(stat,__LINE__,__FILE__);");
	}
        flushcode();
    }
#endif

#ifdef USE_NETCDF4
    /* Construct code to define types*/
    if(ntyps > 0) {
        cline("");
	for(ityp = 0; ityp < ntyps; ityp++) {
	    Symbol* tsym = (Symbol*)listget(typdefs,ityp);
	    if(tsym->subclass == NC_PRIM
		|| tsym->subclass == NC_ARRAY) continue; /* no need to do these*/
	    genc_deftype(tsym);
	    cline("");
	}
    }
    flushcode();
#endif

    /* define dimensions from info in dims array */
    if (ndims > 0) {
	cline("");
	clined(1,"/* define dimensions */");
        for(idim = 0; idim < ndims; idim++) {
            Symbol* dsym = (Symbol*)listget(dimdefs,idim);
    	    nprintf(stmt,sizeof(stmt),
		"    stat = nc_def_dim(%s, \"%s\", %s_len, &%s);",
		groupncid(dsym->container),
                dsym->name, cname(dsym), dimncid(dsym));
	    cline(stmt);
	    clined(1,"check_err(stat,__LINE__,__FILE__);");
       }
    }
    flushcode();

    /* define variables from info in vars array */
    if (nvars > 0) {
	cline("");
	clined(1,"/* define variables */");
	for(ivar = 0; ivar < nvars; ivar++) {
            Symbol* vsym = (Symbol*)listget(vardefs,ivar);
            Symbol* basetype = vsym->typ.basetype;
	    Dimset* dimset = &vsym->typ.dimset;
	    cline("");
	    if(dimset->ndims > 0) {
	        for(idim = 0; idim < dimset->ndims; idim++) {
		    Symbol* dsym = dimset->dimsyms[idim];
		    nprintf(stmt,sizeof(stmt),
			    "    %s_dims[%d] = %s;",
			    cname(vsym),
			    idim,
			    dimncid(dsym));
		    cline(stmt);
		}
	    }
	    nprintf(stmt,sizeof(stmt),
			"    stat = nc_def_var(%s, \"%s\", %s, RANK_%s, %s, &%s);",
		        groupncid(vsym->container),
			vsym->name,
			typencid(basetype),
			cname(vsym),
			(dimset->ndims == 0?"0":poolcat(cname(vsym),"_dims")),
			varncid(vsym));
	    cline(stmt);
	    clined(1,"check_err(stat,__LINE__,__FILE__);");
#ifdef USE_NETCDF4
	    definespecialattributes(vsym);
#endif /*USE_NETCDF4*/
	}
    }
    flushcode();
    
    /* Define the global attributes*/
    if(ngatts > 0) {
	cline("");
	clined(1,"/* assign global attributes */");
	for(iatt = 0; iatt < ngatts; iatt++) {
	    Symbol* gasym = (Symbol*)listget(gattdefs,iatt);
	    cloadattribute(gasym);	    
	}
	cline("");
    }
    flushcode();
    
    /* Define the variable specific attributes*/
    if(natts > 0) {
	cline("");
	clined(1,"/* assign per-variable attributes */");
	for(iatt = 0; iatt < natts; iatt++) {
	    Symbol* asym = (Symbol*)listget(attdefs,iatt);
	    cloadattribute(asym);
	}
	cline("");
    }
    flushcode();

    if (nofill_flag) {
        clined(1,"/* don't initialize variables with fill values */");
	clined(1,"stat = nc_set_fill(%s, NC_NOFILL, 0);");
	clined(1,"check_err(stat,__LINE__,__FILE__);");
    }

    cline("");
    clined(1,"/* leave define mode */");
    nprintf(stmt,sizeof(stmt),"    stat = nc_enddef (%s);",groupncid(rootgroup));
    cline(stmt);
    clined(1,"check_err(stat,__LINE__,__FILE__);");
    flushcode();

    /* Load values into those variables with defined data */

    if(nvars > 0) {
	cline("");
	clined(1,"/* assign variable data */");
        for(ivar = 0; ivar < nvars; ivar++) {
	    Symbol* vsym = (Symbol*)listget(vardefs,ivar);
	    if(vsym->data != NULL) cloadvar(vsym);
	}
	cline("");
	/* compute the max actual size of the unlimited dimension*/
        if(usingclassic) computemaxunlimited();
    }
    flushcode();
}

#ifdef USE_NETCDF4
static void
definespecialattributes(Symbol* vsym)
{
    Specialdata* special = &vsym->var.special;
    if(usingclassic) return;
    if(special->flags & _STORAGE_FLAG) {
        int storage = special->_Storage;
        size_t* chunks = special->_ChunkSizes;
        nprintf(stmt,sizeof(stmt),
                "    stat = nc_def_var_chunking(%s, %s, %s, ",
                groupncid(vsym->container),
                varncid(vsym),
                (storage == NC_CONTIGUOUS?"NC_CONTIGUOUS":"NC_CHUNKED"));
        cpartial(stmt);
        if(special->nchunks == 0 || chunks == NULL)
            cpartial("NULL");
        else {
            nprintf(stmt,sizeof(stmt),"%s_chunksizes",cname(vsym));
            cpartial(stmt);                 
        }
        cline(");");
        clined(1,"check_err(stat,__LINE__,__FILE__);");
    }   
    if(special->flags & _FLETCHER32_FLAG) {
        nprintf(stmt,sizeof(stmt),
                "    stat = nc_def_var_fletcher32(%s, %s, %d);",
                groupncid(vsym->container),
                varncid(vsym),
                special->_Fletcher32);
        cline(stmt);
        clined(1,"check_err(stat,__LINE__,__FILE__);");
    }
    if(special->flags & (_DEFLATE_FLAG | _SHUFFLE_FLAG)) {
        nprintf(stmt,sizeof(stmt),
                "    stat = nc_def_var_deflate(%s, %s, %s, %d, %d);",
                groupncid(vsym->container),
                varncid(vsym),
                (special->_Shuffle == 1?"NC_SHUFFLE":"NC_NOSHUFFLE"),
                (special->_DeflateLevel >= 0?1:0),
                (special->_DeflateLevel >= 0?special->_DeflateLevel:0));
        cline(stmt);
        clined(1,"check_err(stat,__LINE__,__FILE__);");
    }   
    if(special->flags & _ENDIAN_FLAG) {
        nprintf(stmt,sizeof(stmt),
                "    stat = nc_def_var_endian(%s, %s, %s);",
                groupncid(vsym->container),
                varncid(vsym),
                (special->_Endianness == NC_ENDIAN_LITTLE?"NC_ENDIAN_LITTLE"
                                                    :"NC_ENDIAN_BIG")
                );
        cline(stmt);
        clined(1,"check_err(stat,__LINE__,__FILE__);");
    }   
    if(special->flags & _NOFILL_FLAG) {
        nprintf(stmt,sizeof(stmt),
                "    stat = nc_def_var_fill(%s, %s, %s, NULL);",
                groupncid(vsym->container),
                varncid(vsym),
                (special->_Fill?"NC_FILL":"NC_NOFILL")
                );
        cline(stmt);
        clined(1,"check_err(stat,__LINE__,__FILE__);");
    }   
}
#endif /*USE_NETCDF4*/

void
cl_c(void)
{
    nprintf(stmt,sizeof(stmt),"%sstat = nc_close(%s);",indented(1),groupncid(rootgroup));
    cline(stmt);
    clined(1,"check_err(stat,__LINE__,__FILE__);");
#ifndef vms
    clined(1,"return 0;");
#else
    clined(1,"return 1;");
#endif
    cline("}");
    flushcode();
}

/*
 * Output a C statement
 */


#define INDENTMAX 256
static char* dent = NULL;

char*
indented(int n)
{
    char* indentation;
    if(dent == NULL) {
	dent = (char*)emalloc(INDENTMAX+1);
	memset((void*)dent,' ',INDENTMAX);
	dent[INDENTMAX] = '\0';	
    }
    if(n*4 >= INDENTMAX) n = INDENTMAX/4;
    indentation = dent+(INDENTMAX - 4*n);
    return indentation;
}


/* return C name for netCDF type, given type code */
static
const char *
nctype(
     nc_type type)			/* netCDF type code */
{
    switch (type) {
      case NC_CHAR: return "NC_CHAR";
      case NC_BYTE: return "NC_BYTE";
      case NC_SHORT: return "NC_SHORT";
      case NC_INT: return "NC_INT";
      case NC_FLOAT: return "NC_FLOAT";
      case NC_DOUBLE: return "NC_DOUBLE";
      case NC_UBYTE: return "NC_UBYTE";
      case NC_USHORT: return "NC_USHORT";
      case NC_UINT: return "NC_UINT";
      case NC_INT64: return "NC_INT64";
      case NC_UINT64: return "NC_UINT64";
      case NC_STRING: return "NC_STRING";
      default: PANIC("nctype: bad type code");
    }
    return NULL;
}



/*
 * Return C type name for netCDF type, given type code.
 */
static
const char* 
ncctype(
     nc_type type)			/* netCDF type code */
{
    switch (type) {
      case NC_CHAR:
	return "char";
      case NC_BYTE:
	return "signed char";
      case NC_SHORT:
	return "short";
      case NC_INT:
	return "int";
      case NC_FLOAT:
	return "float";
      case NC_DOUBLE:
	return "double";
      case NC_UBYTE:
	return "unsigned char";
      case NC_USHORT:
	return "unsigned short";
      case NC_UINT:
	return "unsigned int";
      case NC_INT64:
	return "signed long long";
      case NC_UINT64:
	return "unsigned long long";
      case NC_STRING:
	return "char*";
      default:
	FPANIC("ncctype: bad type code:%d",type);
    }
    return 0;
}

/*
 * Return C type name for netCDF type suffix, given type code.
 */
static
const char* 
ncstype(
     nc_type nctype)			/* netCDF type code */
{
    switch (nctype) {
      case NC_CHAR:
	return "text";
      case NC_BYTE:
	return "schar";
      case NC_SHORT:
	return "short";
      case NC_INT:
	return "int";
      case NC_FLOAT:
	return "float";
      case NC_DOUBLE:
	return "double";
      case NC_UBYTE:
	return "ubyte";
      case NC_USHORT:
	return "ushort";
      case NC_UINT:
	return "uint";
      case NC_INT64:
	return "longlong";
      case NC_UINT64:
	return "ulonglong";
      case NC_STRING:
	return "string";
      default:
	derror("ncstype: bad type code: %d",nctype);
	return 0;
    }
}

/* Return the group name for the specified group*/
static const char*
groupncid(Symbol* sym)
{
    if(usingclassic) {
        return "ncid";
#ifdef USE_NETCDF4
    } else {
        char* grptmp;
	const char* tmp1;
        if(sym == NULL) return groupncid(rootgroup);
        assert(sym->objectclass == NC_GRP);
        tmp1 = cname(sym);
        grptmp = poolalloc(strlen(tmp1)+strlen("_grp")+1);
        strcpy(grptmp,tmp1);
        strcat(grptmp,"_grp");
        return grptmp;
#endif
    }
}

/* Compute the C name for a given type's id*/
/* Watch out: the result is a static*/
static const char*
typencid(Symbol* tsym)
{
    char* typtmp;
    const char* tmp1;
    if(tsym->subclass == NC_PRIM)
	return nctype(tsym->typ.typecode);
    tmp1 = ctypename(tsym);
    typtmp = poolalloc(strlen(tmp1)+strlen("_typ")+1);
    strcpy(typtmp,tmp1);
    strcat(typtmp,"_typ");
    return typtmp;
}

/* Compute the C name for a given var's id*/
/* Watch out: the result is a static*/
static const char*
varncid(Symbol* vsym)
{
    const char* tmp1;
    char* vartmp;
    tmp1 = cname(vsym);
    vartmp = poolalloc(strlen(tmp1)+strlen("_id")+1);
    strcpy(vartmp,tmp1);
    strcat(vartmp,"_id");
    return vartmp;
}

/* Compute the C name for a given dim's id*/
/* Watch out: the result is a static*/
static const char*
dimncid(Symbol* dsym)
{
    const char* tmp1;
    char* dimtmp;
    tmp1 = cname(dsym);
    dimtmp = poolalloc(strlen(tmp1)+strlen("_dim")+1);
    strcpy(dimtmp,tmp1);
    strcat(dimtmp,"_dim");
    return dimtmp;
}

/* Compute the C name for a given type*/
const char*
ctypename(Symbol* tsym)
{
    const char* name;
    assert(tsym->objectclass == NC_TYPE);
    if(tsym->subclass == NC_PRIM)
	name = ncctype(tsym->typ.typecode);
    else
        name = cname(tsym);
    return name;
}

/* Compute the C name for a given symbol*/
/* Cache in symbol->lname*/
const char*
cname(Symbol* sym)
{
    if(sym->lname == NULL) {
	char* name = pooldup(sym->name);
#ifdef USE_NETCDF4
	if(sym->subclass == NC_FIELD || sym->subclass == NC_ECONST) {
	     sym->lname = nulldup(decodify(name));
	} else
#endif
	if(sym->objectclass == NC_ATT && sym->att.var != NULL) {
	    /* Attribute name must be prefixed with the cname of the*/
	    /* associated variable*/
	    const char* vname = cname(sym->att.var);
	    const char* aname = decodify(name);
	    sym->lname = (char*)emalloc(strlen(vname)
					+strlen(aname)
					+1+1);
	    sym->lname[0] = '\0';
            strcpy(sym->lname,vname);
	    strcat(sym->lname,"_");
	    strcat(sym->lname,aname);
	} else {
            /* convert to language form*/
#ifdef USE_NETCDF4
            sym->lname = nulldup(decodify(cprefixed(sym->prefix,name,"_")));
#else
            sym->lname = nulldup(decodify(name)); /* convert to usable form*/
#endif
	}
    }
    return sym->lname;
}

#ifdef USE_NETCDF4
/* Result is pool alloc'd*/
static char*
cprefixed(List* prefix, char* suffix, char* separator)
{
    int slen;
    int plen;
    int i;
    char* result;

    assert(suffix != NULL);
    plen = prefixlen(prefix);
    if(prefix == NULL || plen == 0) return decodify(suffix);
    /* plen > 0*/
    slen = 0;
    for(i=0;i<plen;i++) {
	Symbol* sym = (Symbol*)listget(prefix,i);
	slen += (strlen(sym->name)+strlen(separator));
    }
    slen += strlen(suffix);
    slen++; /* for null terminator*/
    result = poolalloc(slen);
    result[0] = '\0';
    /* Leave off the root*/
    i = (rootgroup == (Symbol*)listget(prefix,0))?1:0;
    for(;i<plen;i++) {
	Symbol* sym = (Symbol*)listget(prefix,i);
        strcat(result,sym->name); /* append "<prefix[i]/>"*/
	strcat(result,separator);
    }    
    strcat(result,suffix); /* append "<suffix>"*/
    return result;
}

static void
definectype(Symbol* tsym)
{
    int i,j;

    assert(tsym->objectclass == NC_TYPE);
    switch (tsym->subclass) {
    case NC_PRIM: break; /* these are already taken care of*/
    case NC_OPAQUE:
	nprintf(stmt,sizeof(stmt),"typedef unsigned char %s[%lu];",
		cname(tsym), tsym->typ.size);
	cline(stmt);
	break;
    case NC_ENUM:
	stmt[0] = 0;
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* econst = (Symbol*)listget(tsym->subnodes,i);
	    assert(econst->subclass == NC_ECONST);
	    nprintf(stmt,sizeof(stmt),"#define %s ((%s)%s)",
		    cname(econst),
		    ctypename(econst->typ.basetype),
		    cconst(&econst->typ.econst));
	    cline(stmt);
	}
	nprintf(stmt,sizeof(stmt),"typedef %s %s;",
		ctypename(tsym->typ.basetype), cname(tsym));
	cline(stmt);
	break;
    case NC_VLEN:
	nprintf(stmt,sizeof(stmt),"typedef nc_vlen_t %s;\n",
		ctypename(tsym));
	cline(stmt);
	break;
    case NC_COMPOUND:
	nprintf(stmt,sizeof(stmt),"typedef struct %s {",cname(tsym));
	cline(stmt);
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* efield = (Symbol*)listget(tsym->subnodes,i);
	    assert(efield->subclass == NC_FIELD);
	    nprintf(stmt,sizeof(stmt),"%s%s %s",
			indented(1),ctypename(efield->typ.basetype),cname(efield));
	    cpartial(stmt);
	    /* compute any dimension specification*/
	    if(efield->typ.dimset.ndims > 0) {
		Bytebuffer* dimbuf = bbNew();
	        for(j=0;j<efield->typ.dimset.ndims;j++) {
		    Symbol* dim;
		    char tmp[64];
		    bbCat(dimbuf,"[");
		    dim = efield->typ.dimset.dimsyms[j];
		    assert(dim->dim.isconstant);
		    sprintf(tmp,"%u",dim->dim.size);
		    bbCat(dimbuf,tmp);
		    bbCat(dimbuf,"]");
		}
		cprint(dimbuf);
		bbFree(dimbuf);
	    }
	    cline(";");
	}
	nprintf(stmt,sizeof(stmt),"} %s;", ctypename(tsym));
	cline(stmt);
	break;

    case NC_ARRAY:
	/* ignore: this will be handled by def_var*/
	break;

    default: panic("definectype: unexpected type subclass: %d",tsym->subclass);
    }
}

/*
Generate the C code for defining a given type
*/
static void
genc_deftype(Symbol* tsym)
{
    int i;

    assert(tsym->objectclass == NC_TYPE);
    switch (tsym->subclass) {
    case NC_PRIM: break; /* these are already taken care of*/
    case NC_OPAQUE:
	nprintf(stmt,sizeof(stmt),"%sstat = nc_def_opaque(%s, %lu, \"%s\", &%s);",
		indented(1),
		groupncid(tsym->container),
		tsym->typ.size,
		tsym->name,
		typencid(tsym));
	cline(stmt);
	clined(1,"check_err(stat,__LINE__,__FILE__);");
	break;
    case NC_ENUM:
	clined(1,"{");
	nprintf(stmt,sizeof(stmt),"%s econst;",
		ncctype(tsym->typ.basetype->typ.typecode));
	clined(1,stmt);
	nprintf(stmt,sizeof(stmt),"stat = nc_def_enum(%s, %s, \"%s\", &%s);",
		groupncid(tsym->container),
		nctype(tsym->typ.basetype->typ.typecode),
		tsym->name,
		typencid(tsym));
	clined(1,stmt);
	clined(1,"check_err(stat,__LINE__,__FILE__);");
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* econst = (Symbol*)listget(tsym->subnodes,i);
	    assert(econst->subclass == NC_ECONST);
	    nprintf(stmt,sizeof(stmt),"econst = %s;",cconst(&econst->typ.econst));
	    clined(1,stmt);
	    nprintf(stmt,sizeof(stmt),"stat = nc_insert_enum(%s, %s, \"%s\", &econst);",
		    groupncid(tsym->container), typencid(tsym), econst->name);
	    clined(1,stmt);
	}
	clined(1,"}");
	break;
    case NC_VLEN:
	nprintf(stmt,sizeof(stmt),"stat = nc_def_vlen(%s, \"%s\", %s, &%s);",
		groupncid(tsym->container),
		tsym->name,
		typencid(tsym->typ.basetype),
		typencid(tsym));
	clined(1,stmt);
	clined(1,"check_err(stat,__LINE__,__FILE__);");
	break;
    case NC_COMPOUND:
	nprintf(stmt,sizeof(stmt),"stat = nc_def_compound(%s, sizeof(%s), \"%s\", &%s);",
		groupncid(tsym->container),
		ctypename(tsym),
		tsym->name,
		typencid(tsym));
	clined(1,stmt);
	clined(1,"check_err(stat,__LINE__,__FILE__);");
	/* Generate the field dimension constants*/
	clined(1,"{");
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    int j;
	    Symbol* efield = (Symbol*)listget(tsym->subnodes,i);
	    assert(efield->subclass == NC_FIELD);
	    if(efield->typ.dimset.ndims == 0) continue;	    
	    stmt[0] = '\0';
	    nprintf(stmt,sizeof(stmt),"static int %s_dims[%d] = {",
			cname(efield),efield->typ.dimset.ndims);
	    for(j=0;j<efield->typ.dimset.ndims;j++) {
		char tmp[256];
	        Symbol* e = efield->typ.dimset.dimsyms[j];
		assert(e->dim.isconstant);
		sprintf(tmp,"%u",e->dim.size);
		strcat(stmt,(j==0?"":", "));
		strcat(stmt,tmp);
	    }
	    strcat(stmt,"};");
	    clined(1,stmt);
	}
	for(i=0;i<listlength(tsym->subnodes);i++) {
	    Symbol* efield = (Symbol*)listget(tsym->subnodes,i);
	    char tmp[1024];
	    assert(efield->subclass == NC_FIELD);
#ifdef TESTALIGNMENT
	    sprintf(tmp,"%lu",efield->typ.offset);
#else
	    sprintf(tmp,"NC_COMPOUND_OFFSET(%s,%s)",
			ctypename(tsym), cname(efield));
#endif
	    if(efield->typ.dimset.ndims > 0){ 
	        nprintf(stmt,sizeof(stmt),"stat = nc_insert_array_compound(%s, %s, \"%s\", %s, %s, %d, %s_dims);",
		    groupncid(tsym->container),
		    typencid(tsym),
		    efield->name,
		    tmp,
		    typencid(efield->typ.basetype),
		    efield->typ.dimset.ndims,
		    cname(efield));
	    } else {
	        nprintf(stmt,sizeof(stmt),"stat = nc_insert_compound(%s, %s, \"%s\", %s, %s);",
		    groupncid(tsym->container),
		    typencid(tsym),
		    efield->name,
		    tmp,
		    typencid(efield->typ.basetype));
	    }
	    clined(1,stmt);
	    clined(1,"check_err(stat,__LINE__,__FILE__);");
	}
	clined(1,"}");
	break;

    case NC_ARRAY:
	/* ignore: this will be handled by def_var*/
	break;

    default: panic("genc_deftype: unexpected type subclass: %d",tsym->subclass);
    }
}

#endif /*USE_NETCDF4*/

static void
cloadattribute(Symbol* asym)
{
    unsigned long len;
    Datalist* list;
    Symbol* basetype = asym->typ.basetype;
    Bytebuffer* code; /* capture other decls*/

    list = asym->data;
    if(list == NULL) PANIC("empty attribute list");
    len = asym->att.count;
    len = list->length;
    if(len == 0) PANIC("empty attribute list");

    nprintf(stmt,sizeof(stmt),"{ /* %s */",asym->name);
    clined(1,stmt);

/*    nprintf(stmt,sizeof(stmt),"%sstatic const %s %s_att[%ld] = {",indented(1),*/
    nprintf(stmt,sizeof(stmt),"%sstatic const %s %s_att[%ld] = ",indented(1),
			ctypename(basetype),
			cname(asym),
			asym->att.count
			);
    code = bbNew();
    bbCat(code,stmt);
    cprint(code);
    bbClear(code);
    genc_datalist(asym,asym->data,NULL,code);
    cprint(code);
/*    cline("} ;");*/
    bbFree(code);

    /* Use the specialized put_att_XX routines if possible*/
    if(isprim(basetype->typ.typecode)) {
	if(basetype->typ.typecode == NC_STRING) {
            nprintf(stmt,sizeof(stmt),"stat = nc_put_att_string(%s, %s, \"%s\", %lu, %s_att);",
		groupncid(asym->container),
		(asym->att.var == NULL?"NC_GLOBAL"
			              :varncid(asym->att.var)),
		asym->name,
	 	len,
		cname(asym));
	    clined(1,stmt);
	} else {
            nprintf(stmt,sizeof(stmt),"stat = nc_put_att_%s(%s, %s, \"%s\", %s, %lu, %s_att);",
		ncstype(basetype->typ.typecode),
		groupncid(asym->container),
		(asym->att.var == NULL?"NC_GLOBAL"
			              :varncid(asym->att.var)),
		asym->name,
		typencid(basetype),		
	 	len,
		cname(asym));
	    clined(1,stmt);
        }
    } else { /* use the generic put_attribute for user defined types*/
        nprintf(stmt,sizeof(stmt),"stat = nc_put_att(%s, %s, \"%s\", %s, %lu, %s_att);",
		groupncid(asym->container),
		(asym->att.var == NULL?"NC_GLOBAL"
			              :varncid(asym->att.var)),
		asym->name,
		typencid(basetype),
	        len,
		cname(asym));
        clined(1,stmt);
    }
    clined(1,"check_err(stat,__LINE__,__FILE__);");
    clined(1,"}");
}

void
cprint(Bytebuffer* buf)
{
   bbAppendn(ccode,bbContents(buf),bbLength(buf));
}

void
cpartial(char* line)
{
    bbCat(ccode,line);
}

void
cline(char* line)
{
    cpartial(line);
    bbCat(ccode,"\n");
}

void
clined(int n, char* line)
{
    bbCat(ccode,indented(n));
    cline(line);
}

void
flushcode(void)
{
    if(bbLength(ccode) > 0) {
        bbAppend(ccode,'\0');
        fputs(bbContents(ccode),stdout);
        fflush(stdout);
        bbClear(ccode);
    }
}

static void
computemaxunlimited(void)
{
    int i;
    unsigned long maxsize;
    Symbol* udim = rootgroup->grp.unlimiteddim;
    if(udim == NULL) return; /* there is no unlimited dimension*/
    /* Look at each variable and see what*/
    /* size it gives to the unlimited dim (if any)*/
    maxsize = 0;
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* dim;
	Symbol* var = (Symbol*)listget(vardefs,i);	
	if(var->typ.dimset.ndims == 0) continue; /* rank == 0*/
	dim = var->typ.dimset.dimsyms[0];
	if(dim->dim.size != NC_UNLIMITED) continue; /* var does not use unlimited*/
	if(var->typ.dimset.dimsyms[0]->dim.size > maxsize)
	    maxsize = var->typ.dimset.dimsyms[0]->dim.size;
    }
}


/* Define the put_vara closure function for C data*/
/*
Important assumptions:
1. The leftmost changed index controls the count set.
2. All indices to the right of #1 are assumed to be at there
   max values.
*/

static int
cputvara(struct Putvar* closure, Odometer* odom, Bytebuffer* databuf)
{
    int i;
    int stat = NC_NOERR;
    size_t startset[NC_MAX_DIMS];
    size_t countset[NC_MAX_DIMS];
    char dimstring[1+(NC_MAX_DIMS*(2+32))];
    Symbol* vsym = closure->c.var;
    Symbol* basetype = vsym->typ.basetype;
    Dimset* dimset = &vsym->typ.dimset;
    Bytebuffer* code = closure->code;

#ifdef F
fprintf(stderr,"putvara: %s: ",vsym->name);
fprintf(stderr,"odom = %s\n",odometerprint(odom));
fprintf(stderr,"initial startset = [");
for(i=0;i<closure->rank;i++) fprintf(stderr,"%s%u",(i>0?", ":""),closure->startset[i]);
fprintf(stderr,"]\n");
fflush(stderr);
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

    /* generate constants for data*/
    dimstring[0] = '\0';
    for(i=0;i<dimset->ndims;i++) {
	char digits[32];
/*odom->dims[i]*/
	sprintf(digits,"[%lu]",countset[i]);
	strcat(dimstring,digits);	
    }

    /* define a block to avoid name clashes*/
    nprintf(stmt,sizeof(stmt),"%s{\n",indented(1));
    bbCat(code,stmt);    

    nprintf(stmt,sizeof(stmt),"%s%s %s_data%s = ",indented(1),
			    ctypename(basetype),
			    cname(vsym),
			    dimstring);
    bbCat(code,stmt);
    bbAppendn(code,bbContents(databuf),bbLength(databuf));
    bbCat(code," ;\n");

    /* generate constants for startset, countset*/
    nprintf(stmt,sizeof(stmt),"%ssize_t %s_startset[%lu] = {",indented(1),
			    cname(vsym),
			    closure->rank);
    bbCat(code,stmt);
    for(i=0;i<closure->rank;i++) {
        nprintf(stmt,sizeof(stmt),"%s%lu",(i>0?", ":""),startset[i]);
	bbCat(code,stmt);
    }
    bbCat(code,"} ;\n");

    nprintf(stmt,sizeof(stmt),"%ssize_t %s_countset[%lu] = {",indented(1),
			    cname(vsym),
			    closure->rank);
    bbCat(code,stmt);
    for(i=0;i<closure->rank;i++) {
        nprintf(stmt,sizeof(stmt),"%s%lu",(i>0?", ":""),countset[i]);
	bbCat(code,stmt);
    }
    bbCat(code,"} ;\n");

    nprintf(stmt,sizeof(stmt),"%sstat = nc_put_vara(%s, %s, %s_startset, %s_countset, %s_data);\n",
		indented(1),
		groupncid(vsym->container), varncid(vsym),
		cname(vsym),
		cname(vsym),
		cname(vsym));
    bbCat(code,stmt);
    nprintf(stmt,sizeof(stmt),"%scheck_err(stat,__LINE__,__FILE__);\n",indented(1));
    bbCat(code,stmt);

    /* end defined block*/
    nprintf(stmt,sizeof(stmt),"%s}\n",indented(1));
    bbCat(code,stmt);    

    for(i=0;i<closure->rank;i++) {
        closure->startset[i] = startset[i] + countset[i];
    }
done:
    bbClear(databuf);
    return stat;
}


static void
cloadvar(Symbol* vsym)
{
    Dimset* dimset = &vsym->typ.dimset;
    Symbol* basetype = vsym->typ.basetype;
    int rank = dimset->ndims;
    int isscalar = (dimset->ndims == 0);
    int isclassic = classicunlimited(dimset);/* all bounded or only dim[0]==UNLIMITED*/
    Bytebuffer* code;

    if(vsym->data == NULL) return;

    code = bbNew();

    /* Handle special cases first*/
    if(isscalar) {
	genc_datalist(vsym,vsym->data,NULL,code);
	clined(1,"{");
/*        nprintf(stmt,sizeof(stmt),"static %s %s_data[1] = {%s};\n",*/
        nprintf(stmt,sizeof(stmt),"static %s %s_data[1] = %s;\n",
			    ctypename(basetype),
			    cname(vsym),
			    bbContents(code));
	clined(1,stmt);
        nprintf(stmt,sizeof(stmt),"stat = nc_put_var(%s, %s, %s_data);",
		groupncid(vsym->container),
		varncid(vsym),
		cname(vsym));
        clined(1,stmt);
        clined(1,"check_err(stat,__LINE__,__FILE__);");
	clined(1,"}");
    } else { /* Non-scalar*/
        /* build a closure*/
	Putvar closure;
        closure.putvar = cputvara;
        closure.rank = rank;
	closure.code = code;
        closure.c.var = vsym;
        memset(closure.startset,0,sizeof(closure.startset));

	{
	    /* Most complex case; use closure as needed*/
	    /* Use a separate data buffer to capture the data list*/
	    Bytebuffer* databuf = bbNew();
	    genc_datalist(vsym,vsym->data,&closure,databuf);
	    cprint(code);
	    bbFree(databuf);
	}
    }
    bbFree(code);    
}
