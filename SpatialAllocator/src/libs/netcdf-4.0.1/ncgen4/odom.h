#ifndef ODOM_H
#define ODOM_H 1

typedef struct Dimdata {
    unsigned long datasize; /* actual size of the datalist item*/
    unsigned long index;    /* 0 <= index < datasize*/
    unsigned long declsize;
} Dimdata;

typedef struct Odometer {
    int     rank;
    Dimdata dims[NC_MAX_DIMS];
} Odometer;

/* Odometer operators*/
extern Odometer* newodometer(Dimset*);
extern void freeodometer(Odometer*);
extern char* odometerprint(Odometer* odom);

extern int odometermore(Odometer* odom);
extern int odometerincr(Odometer* odo,int);
extern unsigned long odometercount(Odometer* odo);
extern void odometerreset(Odometer*);
extern unsigned long odometertotal(Odometer*);
extern Odometer* odometersplit(Odometer* odom, int tail);

#endif /*ODOM_H*/
