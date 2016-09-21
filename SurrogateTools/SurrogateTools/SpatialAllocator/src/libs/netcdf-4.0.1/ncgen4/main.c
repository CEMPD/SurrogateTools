/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/main.c,v 1.6 2009/03/11 18:26:18 dmh Exp $
 *********************************************************************/
#include "includes.h"
#include "offsets.h"

extern void init_netcdf(void);
extern void parse_init(void);
extern int  yyparse(void);

/* For error messages */
char* progname;
char* cdlname;

int kflag_flag; /* 1 => -k was specified on command line*/
int cmode_modifier;
int nofill_flag;
char structformat_flag;
char* mainname; /* name to use for main function; defaults to "main"*/
int c_flag;
int netcdf_flag;
int fortran_flag;

char *netcdf_name; /* command line -o file name */
char *datasetname; /* name from the netcdf <name> {} */

/* Misc. flags*/
int usingclassic;

extern FILE *yyin;

/* Forward */
char* ubasename ( const char* av0 );
void usage ( void );
int main ( int argc, char** argv );

/* Define tables vs modes for legal -k values*/
#define NKVALUES 14
struct Kvalues {
char* name;
int mode;
} legalkinds[NKVALUES] = {
    {"1", 0},
    {"classic", 0},

/* The 64-bit offset kind (2)  should only be used if actually needed */
    {"2", NC_64BIT_OFFSET},
    {"64-bit-offset", NC_64BIT_OFFSET},
    {"64-bit offset", NC_64BIT_OFFSET},

    /* NetCDF-4 HDF5 format*/
    {"3", NC_NETCDF4},
    {"hdf5", NC_NETCDF4},
    {"netCDF-4", NC_NETCDF4},
    {"enhanced", NC_NETCDF4},

    /* NetCDF-4 HDF5 format, but using only nc3 data model */
    {"4", NC_NETCDF4 | NC_CLASSIC_MODEL},
    {"hdf5-nc3", NC_NETCDF4 | NC_CLASSIC_MODEL},
    {"netCDF-4 classic model", NC_NETCDF4 | NC_CLASSIC_MODEL},
    {"enhanced-nc3", NC_NETCDF4 | NC_CLASSIC_MODEL},

    /* null terminate*/
    {NULL,0}
};

/* strip off leading path */
/* result is malloc'd */
char *
ubasename(const char *av0)
{
	char *logident = nulldup(av0);
#ifdef VMS
#define SEP	']'
#endif
#ifdef MSDOS
#define SEP	'\\'
#endif
#ifndef SEP
#define SEP	'/'
#endif
	if ((logident = strrchr(logident, SEP)) != NULL)
	    logident++ ;
	return logident ;
}

void
usage(void)
{
    derror("Usage: %s [ -b ] [ -c ] [ -f ] [ -k kind ] [ -x ] [-S struct-format] [-M <name> [ -o outfile]  [ file ... ]",
	   progname);
    derror("netcdf library version %s", nc_inq_libvers());
}

/* These are in unistd.h; for use with getopt()
extern int optind;
extern int opterr;
extern char *optarg;
*/

int
main(
	int argc,
	char *argv[])
{
    int c;
    FILE *fp;

#ifdef __hpux
    setlocale(LC_CTYPE,"");
#endif
    
    init_netcdf();

    opterr = 1;			/* print error message if bad option */
    progname = ubasename(argv[0]);
    cdlname = "-";
    netcdf_name = NULL;
    datasetname = NULL;
    c_flag = 0;
    fortran_flag = 0;
    netcdf_flag = 0;
    kflag_flag = 0;
    cmode_modifier = 0;
    nofill_flag = 0;
    structformat_flag = 'p';
    mainname = "main";

#if _CRAYMPP && 0
    /* initialize CRAY MPP parallel-I/O library */
    (void) par_io_init(32, 32);
#endif

    while ((c = getopt(argc, argv, "bcfk:l:no:v:xS:dM:D:")) != EOF)
      switch(c) {
	case 'd':
	  debug = 1;	  
	  break;
	case 'D':
	  debug = atoi(optarg);
	  break;
	case 'c': /* for c output, old version of "-lc" */
	  c_flag = 1;
	  break;
	case 'f': /* for fortran output, old version of "-lf" */
	  fortran_flag = 1;
	  break;
	case 'b': /* for binary netcdf output, ".nc" extension */
	  netcdf_flag = 1;
	  break;
        case 'l': /* specify language, instead of using -c or -f */
	    {
		char *lang_name = (char *) emalloc(strlen(optarg)+1);
		(void)strcpy(lang_name, optarg);
		if (strcmp(lang_name, "c") == 0
                    || strcmp(lang_name, "C") == 0) {c_flag = 1;}
		else if (strcmp(lang_name, "f77") == 0 || 
			 strcmp(lang_name, "fortran77") == 0 ||
			 strcmp(lang_name, "Fortran77") == 0) {
		    fortran_flag = 1;
		} else { /* Fortran90, Java, C++, Perl, Python, Ruby, ... */
		    derror("%s: output language %s not implemented", 
			   progname, lang_name);
		    return(1);
		}
	    }
	  break;
	case 'n':		/* old version of -b, uses ".cdf" extension */
	  netcdf_flag = -1;
	  break;
	case 'o':		/* to explicitly specify output name */
/*	  netcdf_flag = 1;*/
	  netcdf_name = strdup(optarg);
	  break;
	case 'x': /* set nofill mode to speed up creation of large files */
	  nofill_flag = 1;
	  break;
        case 'v': /* a deprecated alias for "kind" option */
	    /*FALLTHRU*/
        case 'k': /* for specifying variant of netCDF format to be generated 
                     Possible values are:
                     1 (=> classic 32 bit)
                     2 (=> classic 64 bit)
                     3 (=> enhanced)
                     4 (=> classic, but stored in an enhanced file format)
                     Also provide string versions of above
                     "classic"
                     "64-bit-offset"
                     "64-bit offset"
		     "enhanced" | "hdf5" | "netCDF-4"
                     "enhanced-nc3" | "hdf5-nc3" | "netCDF-4 classic model"
		   */
	    {
		struct Kvalues* kvalue;
		char *kind_name = (char *) emalloc(strlen(optarg)+1);
		if (! kind_name) {
		    derror ("%s: out of memory", progname);
		    return(1);
		}
		(void)strcpy(kind_name, optarg);
	        for(kvalue=legalkinds;kvalue->name;kvalue++) {
		    if(strcmp(kind_name,kvalue->name) == 0) {
		        cmode_modifier = kvalue->mode;
			break;
		    }
		}
		if(kvalue->name == NULL) {
		   derror("Invalid format: %s",kind_name);
		   return 2;
		}
		kflag_flag = 1;
	    }
	  break;
	case 'M': /* Determine the name for the main function */
	    mainname = strdup(optarg);
	    break;
	case 'S': /* Determine the format for struct layout */
	  if(strcmp(optarg,"pack") == 0) {
	      structformat_flag = 'p';
	  } else if(strcmp(optarg,"max") == 0) {
	      structformat_flag = 'm';
	  } else {
	    /* Eventually add formats for C layout (and fortran?) */
	    derror("Illegal -S format; must be one of: pack or max");
	    return 2;
	  }
	  break;
	case '?':
	  usage();
	  return(8);
      }

#ifndef ENABLE_C
    if(c_flag) {
	  fprintf(stderr,"C not currently supported\n");
	  exit(1);
    }
#endif
#ifndef ENABLE_FORTRAN
    if(fortran_flag) {
	  fprintf(stderr,"FORTRAN not currently supported\n");
	  exit(1);
    }
#endif
#ifndef ENABLE_CDF
    if(netcdf_flag) {
	  fprintf(stderr,"Direct netcdf not currently supported\n");
	  exit(1);
    }
#endif
    if(CLASSICONLY && (cmode_modifier & NC_NETCDF4) != 0) {
        derror("compiled for CLASSIC only; enhanced model not supported for -k flag");
        return (9);
    }

    if(fortran_flag && (cmode_modifier & NC_NETCDF4) != 0) {
        derror("Fortran not currently supported for enhanced (netCDF-4) model");
        return (9);
    }

    if(fortran_flag && c_flag) {
	derror("Only one of -lc or -lf may be specified");
	return(8);
      }

    netcdf_flag = (!fortran_flag && !c_flag);

    argc -= optind;
    argv += optind;

    if (argc > 1) {
	derror ("%s: only one input file argument permitted",progname);
	return(6);
    }

    fp = stdin;
    if (argc > 0 && strcmp(argv[0], "-") != 0) {
	if ((fp = fopen(argv[0], "r")) == NULL) {
	    derror ("can't open file %s for reading: ", argv[0]);
	    perror("");
	    return(7);
	}
	cdlname = (char*)emalloc(NC_MAX_NAME);
	cdlname = strdup(argv[0]);
	if(strlen(cdlname) > NC_MAX_NAME) cdlname[NC_MAX_NAME] = '\0';
    }

    /* Initially set up the cmode value and related items*/
    if(kflag_flag == 0) cmode_modifier = NC_NETCDF4;
    usingclassic = ((cmode_modifier & NC_NETCDF4) == 0);

    parse_init();
    yyin = fp;
    if(debug >= 2) {yydebug=1;}
    if(yyparse() != 0) return 1;

    /* Recompute mode flag*/
    usingclassic = ((cmode_modifier & NC_NETCDF4) == 0);

    processsemantics();
    define_netcdf();

    return 0;
}

void
init_netcdf(void) /* initialize global counts, flags */
{
    compute_alignments();

}

