#include "includes.h"
#include "odom.h"

#define POSDONE(pos) ((pos)->index >= (pos)->datasize)

#define ODOMDONE(odom) ((odom)->rank > 0 && !POSDONE(odom->dims))

/**************************************************/
/* Define methods for a dimension odometer*/

Odometer*
newodometer(Dimset* dimset)
{
    int i;
    Odometer* odom = (Odometer*)emalloc(sizeof(Odometer));
    odom->rank = (dimset==NULL?0:dimset->ndims);
    assert(odom->rank <= NC_MAX_DIMS);
    for(i=0;i<odom->rank;i++) {
	odom->dims[i].declsize = dimset->dimsyms[i]->dim.size;
	odom->dims[i].index = 0;
	odom->dims[i].datasize = 0;
    }    
    return odom;
}

void
freeodometer(Odometer* odom)
{
    if(odom) free(odom);
}

char*
odometerprint(Odometer* odom)
{
    int i;
    static char line[1024];
    char tmp[64];
    line[0] = '\0';
    if(odom->rank == 0) {
	strcat(line,"[]");
    } else for(i=0;i<odom->rank;i++) {
	sprintf(tmp,"[%lu/%lu:%lu]",
		odom->dims[i].datasize,
		odom->dims[i].declsize,
		odom->dims[i].index);
	strcat(line,tmp);	
    }
    return line;
}

int
odometermore(Odometer* odom)
{
    return ODOMDONE(odom);
}

unsigned long
odometercount(Odometer* odom)
{
    int i;
    unsigned long offset = odom->dims[0].index;
    for(i=1;i<odom->rank;i++) {
	offset *= odom->dims[i-1].datasize;
	offset += odom->dims[i].index;
    } 
    return offset;
}

void
odometerreset(Odometer* odom)
{
    Dimdata* dimdata = odom->dims;
    int rank = odom->rank;
    while(rank-- > 0) {dimdata->index = 0; dimdata++;};
}

/* Given an odometer compute the total*/
/* number of values it would return.*/
/* This is used to determine the length*/
/* for fill values */

unsigned long
odometertotal(Odometer* odom)
{
    Dimdata* dimdata = odom->dims;
    int rank = odom->rank;
    unsigned long count = 1;
    while(rank-- > 0) {count *= (dimdata->datasize); dimdata++;}
    return count;
}

/*
Return an odometer that covers the last tail
elements in the input odometer and removes
them from the input odometer.
*/
Odometer*
odometersplit(Odometer* odom, int tail)
{
    int i;
    Odometer* split = (Odometer*)emalloc(sizeof(Odometer));
    assert(odom->rank >= tail);
    split->rank = tail;
    odom->rank = odom->rank - tail;
    for(i=0;i<tail;i++) {split->dims[i] = odom->dims[odom->rank+i];}
    return split;
}

int
odometerincr(Odometer* odom, int wheel)
{
    int i;
    Dimdata* dimdata;
    if(odom->rank == 0) return 0; 
    if(wheel < 0) wheel = (odom->rank - 1);
    dimdata = odom->dims+(wheel); /* point to wheel'th entry*/
    for(i=wheel;i>=0;i--,dimdata--) {
        dimdata->index++;
        if(!POSDONE(dimdata)) break;
	if(i == 0) return 0; /* leave the 0th entry if it overflows*/
	dimdata->index = 0;
    }
    return 1;
}
