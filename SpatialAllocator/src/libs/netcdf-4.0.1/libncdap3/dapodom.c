/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapodom.c,v 1.1 2009/03/25 01:48:28 dmh Exp $
 *********************************************************************/

#include "config.h"
#include "ncdap3.h"
#include "dapodom.h"

/**********************************************/
/* Define methods for a dimension dapodometer*/

Dapodometer*
newdapodometer(NCslice* slices, int first, int count)
{
    int i;
    Dapodometer* odom = (Dapodometer*)emalloc(sizeof(Dapodometer));
    MEMCHECK(odom,NULL);
    odom->rank = count;
    assert(odom->rank <= NC_MAX_DIMS);
    for(i=0;i<odom->rank;i++) {
	odom->slices[i] = slices[first+i];
	odom->index[i] = odom->slices[i].first;
    }    
    return odom;
}

void
freedapodometer(Dapodometer* odom)
{
    if(odom) free(odom);
}

char*
dapodometerprint(Dapodometer* odom)
{
    int i;
    static char line[1024];
    char tmp[64];
    line[0] = '\0';
    if(odom->rank == 0) {
	strcat(line,"[]");
    } else for(i=0;i<odom->rank;i++) {
	sprintf(tmp,"[%lu+%lu/%lu:%lu]",
		(unsigned long)odom->slices[i].first,
		(unsigned long)odom->slices[i].count,
		(unsigned long)odom->slices[i].stride,
		(unsigned long)odom->index[i]);
	strcat(line,tmp);	
    }
    return line;
}

int
dapodometermore(Dapodometer* odom)
{
    return (odom->index[0] < odom->slices[0].stop);
}

/* Convert current dapodometer settings to a single integer count*/
size_t
dapodometercount(Dapodometer* odom)
{
    int i;
    size_t offset = odom->index[0];
    for(i=1;i<odom->rank;i++) {
	offset *= odom->slices[i-1].count;
	offset += odom->index[i];
    } 
    return offset;
}

void
dapodometerreset(Dapodometer* odom)
{
    int rank = odom->rank;
    while(rank-- > 0) {odom->index[rank] = odom->slices[rank].first;}
}

/* Given an dapodometer compute the total*/
/* number of values it would return.*/
/* This is used to determine the length*/
/* for fill values */

size_t
dapodometertotal(Dapodometer* odom)
{
    int rank = odom->rank;
    size_t count = 1;
    while(rank-- > 0) {count *= (odom->slices[rank].count);}
    return count;
}

/*
Return an dapodometer that covers the last tail
elements in the input dapodometer and removes
them from the input dapodometer.
*/
Dapodometer*
dapodometersplit(Dapodometer* odom, int tail)
{
    int i;
    Dapodometer* split = (Dapodometer*)emalloc(sizeof(Dapodometer));
    MEMCHECK(split,NULL);
    assert(odom->rank >= tail);
    split->rank = tail;
    odom->rank = odom->rank - tail;
    for(i=0;i<tail;i++) {split->slices[i] = odom->slices[odom->rank+i];}
    return split;
}

int
dapodometerincr(Dapodometer* odom)
{
    return dapodometerincrith(odom,-1);
}

int
dapodometerincrith(Dapodometer* odom, int wheel)
{
    int i;
    NCslice* slice;
    if(odom->rank == 0) return 0; 
    if(wheel < 0) wheel = (odom->rank - 1);
    for(slice=odom->slices+(wheel),i=wheel;i>=0;i--,slice--) {
        odom->index[i] += slice->stride;
        if(odom->index[i] < slice->stop) break;
	if(i == 0) return 0; /* leave the 0th entry if it overflows*/
	odom->index[i] = slice->first; /* reset this position*/
    }
    return 1;
}
