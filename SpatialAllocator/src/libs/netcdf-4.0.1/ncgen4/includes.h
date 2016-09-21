
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#include "config.h"
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>	/* for isprint() */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __hpux
#include <locale.h>
#endif
    
#include "netcdf.h"
#include "ncgen.h"
#include "odom.h"
#include "data.h"
#include "genlib.h"
#include "util.h"
#include "debug.h"
