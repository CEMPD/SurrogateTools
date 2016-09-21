/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/genlib.c,v 1.3 2009/03/11 18:26:17 dmh Exp $
 *********************************************************************/

#include "includes.h"

/* invoke netcdf calls (or generate C or Fortran code) to create netcdf
 * from in-memory structure.
The output file name is chosen by using the following in priority order:
1. -o flag name
2. command line input file with .cdl changed to .nc
3. dataset name as specified in netcdf <name> {...} 
*/
void
define_netcdf(void)
{
    char *filename;		/* output file name */
    filename = NULL;
    if(netcdf_name) { /* -o flag name */
	filename = strdup(netcdf_name);
    }    
    if(filename == NULL &&
	cdlname != NULL && strcmp(cdlname,"-") != 0) {/* cmd line name */
	char base[1024];
	char* p;
	strcpy(base,cdlname);
	/* remove any suffix and prefix*/
	p = strrchr(base,'.');
	if(p != NULL) {*p= '\0';}
	p = strrchr(base,'/');
	if(p != NULL) {strcpy(base,p+1);}
	if(strlen(base) > 0) {
	    strcat(base,
		(netcdf_flag == -1?".cdf" /* old, deprecated extension */
				  :".nc")); /* preferred*/
	    filename = strdup(base);
	}
    }
    if(filename == NULL) {/* construct name from dataset name */
	char base[1024];
	strcpy(base,datasetname);
	strcat(base,
		(netcdf_flag == -1?".cdf" /* old, deprecated extension */
				  :".nc")); /* preferred*/
	filename = strdup(base);
    }
#ifdef ENABLE_C
    if (c_flag) gen_ncc(filename); /* create C code to create netcdf */
#endif
#ifdef ENABLE_CDF
    if (netcdf_flag) gen_netcdf(filename); /* create netcdf */
#endif
#ifdef ENABLE_FORTRAN
    if (fortran_flag) gen_ncfortran(filename); /* create Fortran code */
#endif
    close_netcdf();
    cleanup();
}

void
close_netcdf(void)
{
#ifdef ENABLE_C
    if (c_flag) cl_c();             /* create C code to close netcdf */
#endif
#ifdef ENABLE_CDF
    if (netcdf_flag) cl_netcdf(); /* close netcdf */
#endif
#ifdef ENABLE_FORTRAN
    if (fortran_flag) cl_fortran(); /* create Fortran code to close netcdf */
#endif
}
