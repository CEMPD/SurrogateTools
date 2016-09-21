#ifndef DATA_H
#define DATA_H 1

/* Convenience*/
#define SRCPUSH(iscmpd,src) do{if(((iscmpd)=issublist(src))) srcpush(src);}while(0)
#define SRCPOP(iscmpd,src) do{if((iscmpd)) srcpop(src);}while(0)

/* nmemonic*/
#define TOPLEVEL 1

typedef struct Datalist {
    struct Datalist* next; /* chain of all known datalists*/
    int           readonly; /* data field is shared with another Datalist*/
    unsigned int  length; /* track # entries in data field*/
    unsigned int  alloc;  /* track total allocated space for data field*/
    Constant*     data; /* actual list of constants constituting the datalist*/
    /* Track various values associated with the datalist*/
    /* (used to be in Constvalue.compoundv)*/
    struct Symbol* schema; /* type/var that defines structure of this*/
    struct Vlen {
	unsigned int count; /* # of vlen basetype instances*/
	unsigned int uid;       /* unique id for NC_VLEN*/
    } vlen;
    Odometer* dimdata; /* store the actual sizes of the variable's dimensions*/
} Datalist;

/* Define a closure for doing*/
/* periodic put_vara operations*/

typedef struct Putvar {
    int (*putvar)(struct Putvar*, Odometer*, Bytebuffer*);
    int rank;
    Bytebuffer* code;
    size_t startset[NC_MAX_DIMS];
    struct CDF {
        int grpid;
        int varid;
    } cdf;
    struct C {
	Symbol* var;
    } c;
} Putvar;


int issublist(Datasrc* src);
int isstring(Datasrc* src);
int isfillvalue(Datasrc* src);
int istype(Datasrc* src, nc_type);

#ifdef DEBUG
void report0(char* lead, Datasrc* src, int index);
void report1(char* lead, Datasrc* src, int index);
#endif


/* from: cdata.c */
void genc_datalist(Symbol* sym, Datalist*, Putvar*, Bytebuffer*);
void genc_vlenconstants(Bytebuffer*);

/* from: cdfdata.c */
void gencdf_datalist(Symbol* sym, Datalist*, Bytebuffer*, Putvar*);
void gencdf_vlenconstants(void);

/* from: data.c */
char* cconst(Constant* ci);
Constant gen_string(unsigned long, Datasrc*, Bytebuffer*);
Constant gen_stringn(unsigned long chunksize, Datasrc*, Bytebuffer*);
Constant cloneconstant(Constant* con); /* shallow clone*/

Datasrc makedatasrc(Datalist* list);
void srcpush(Datasrc*);
void srcpushlist(Datasrc* src, Datalist* cmpd);
int srcpop(Datasrc*);

Constant* srcnext(Datasrc*);
int srcmore(Datasrc*);
int srcline(Datasrc* ds);
void srcautopop(Datasrc* ds);

#ifdef FASTDATASRC
#define srcpeek(ds) ((ds)->index >= (ds)->max?NULL:(ds)->data+(ds)->index)
#else
Constant* srcpeek(Datasrc*);
#endif

#endif /*DATA_H*/
