/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdap.h,v 1.3 2009/03/25 17:11:32 dmh Exp $
 *********************************************************************/
#ifndef NCDAP_H
#define NCDAP_H 1

#include "ocinternal.h"
#include "ocdata.h"
#include "occontent.h"
#include "ocdrno.h"
#include "dapurl.h"
#include "ncbytebuffer.h"
#include "nclist.h"
#include "nchashmap.h"

#include "daputil.h"
#include "dapdebug.h"

#define DEFAULTSTRINGLENGTH 64

#ifdef USE_NETCDF4
/* Use an extendef version of the netCDF-4 type system */
#define	NC_URL		32
#define	NC_SET	        33
/* Merge relevant translations of OC types */
#define NC_Dataset	34
#define NC_Sequence	35
#define NC_Structure	36
#define NC_Grid		37
#define NC_Dimension	38
#define NC_Primitive	39
#endif

/* The DAP packet info*/
typedef struct NCDAP {
    OCconnection* conn;
    char* urltext; /* as given to nc3d_open*/
    DAPURL url; /* as given to nc3d_open and parsed*/
} NCDAP;

typedef struct NCCDF {
    struct CDFnode* root;
    NClist*  cdfnodes; /* NClist<CDFnode*> */
    unsigned int defaultstringlength;
    unsigned int defaultsequencelimit; /* global sequence limit; 0 => no limit*/
    /* selected NClist nodes can simulate |unlimited| > 0*/
    struct CDFnode*      unlimited; /* fake unlimited dimension*/
    struct CDFnode*      sequnlimited; /* NClist node representing simulated unlimited*/
    int                  sequnlimitedfield; /* which field of Dataset is unlimited sequence*/
    NChashmap* ocmap; /* map OCnodes to mirror CDFnodes*/
    void*  control;   /* cross link to controlling structure (e.g. NC*) */
} NCCDF;

typedef struct NCVAR {
    struct Getvar* varinfo;
} NCVAR;

typedef struct NCDRNO {
    NC* ncp;
    NCCDF cdf;
    NCDAP dap;
    NCVAR var;
} NCDRNO;

/* Create our own node tree to mimic ocnode trees*/

typedef struct CDFdim {
    struct CDFnode* basedim; /* for duplicate dimensions*/
    size_t declsize;	    /* from DDS*/
} CDFdim;

typedef struct CDFarray {
    /* The complete set of dimension info applicable to this node*/
    NClist*  dimensions; /* NClist<CDFnode*>*/
    /* convenience (because they are computed so often*/
    int rank; /* == |dimensions|*/
} CDFarray;

/* Closely mimics struct OCnode*/
typedef struct CDFnode {
    nc_type          nctype; /* redundant but convenient*/
    nc_type          etype;  /* redundant but convenient*/
    char*            name;   /* redundant but convenient*/
    OCnode*          dds;    /* mirror node*/
    OCnode*          datadds; /* mirror node*/
    struct CDFnode*  container;
    NCDRNO*          owner;
    CDFdim           dim;
    CDFarray         array;
    NClist*          subnodes; /*NClist<OCnode*>*/
    char*            ncbasename;     /* without parent name prefixing, but legitimate */
    char*            ncfullname;     /* with parent name prefixing*/
    nc_type           externaltype;   /* the type as represented to nc_inq*/
    int              ncid;           /* relevant NC id for this object*/
    size_t           maxstringlength;
    size_t           maxsequencelength;
    int              elided;  /* 1 => node does not partipate in naming*/
    /* Fields for use by libncdap4 */
    size_t           instancesize; /* size of single instance of this type*/
    size_t           fieldsize; /* size of field */
    size_t           alignment; /* alignment of this field */
    size_t           offset;    /* offset of this field in parent */
    int              typeid;    /* when treating field as type */
    int              basetypeid;   /* when typeid is vlen */
    char*            typename;
    char*            vlenname; /* for sequence types */
} CDFnode;

/* It is important to track error status as coming from nc or oc*/
typedef int NCerror; /* OCerror is already defined*/

/* From: cdf.c*/
extern void computecdfinfo3(NCDRNO*, NClist*);
extern char* cdfname3(char* basename);
extern void augmentddstree3(NCDRNO*, OClist*);
extern void computecdfdimnames3(NClist*);
extern void computecdfvarnames3(NCDRNO*);
extern CDFnode* buildcdftree3(NCDRNO*, OCnode*, CDFnode*);
extern CDFnode* makecdfnode3(NCDRNO*, char* nm, OCtype,
			    /*optional*/ OCnode* ocnode, CDFnode* container);
extern void freeallcdfnodes3(NClist* nodes);

/* From: error.c*/
extern NCerror ocerrtoncerr(OCerror);

/* From: ncdap3.c*/
extern NCerror nc3d_open(const char* path, int mode, int* ncidp);
extern int nc3d_close(int ncid);
extern void applyclientparams3(NCDRNO* drno);
extern void freeNCDRNO3(NCDRNO* state);
extern void attachdatadds3(struct CDFnode*, struct OCnode*);
extern void detachdatadds3(struct NCDRNO*);
extern int  showcheck(struct NCDRNO*, const char* showparam);
extern char* makecdfpathstring3(CDFnode* var);
extern const char* daplookup(NCDRNO* drno, const char* param);

/* From: dapcvt.c*/
extern NCerror ncdap3convert(nc_type srctype, nc_type dsttype, char* memory, char* value);

/* Add an extra function whose sole purpose is to allow
   configure(.ac) to test for the presence of thiscode.
*/
extern int nc__opendap(void);

#endif /*NCDAP_H*/
