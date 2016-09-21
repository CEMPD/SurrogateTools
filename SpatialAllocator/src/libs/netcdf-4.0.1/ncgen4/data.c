#include        "includes.h"
#include        "bytebuffer.h"

static void gen_stringchunks(unsigned long, Datasrc*, Bytebuffer*);
static void srcpushlist0(Datasrc* src, Datalist* cmpd);

/**************************************************/
/**************************************************/

/* return 1 if the next element in the datasrc is compound*/
int
issublist(Datasrc* src) {return istype(src,NC_COMPOUND);}

/* return 1 if the next element in the datasrc is a string*/
int
isstring(Datasrc* src) {return istype(src,NC_STRING);}

/* return 1 if the next element in the datasrc is a fill value*/
int
isfillvalue(Datasrc* src) {return istype(src,NC_FILLVALUE);}

/* return 1 if the next element in the datasrc is nc_type*/
int
istype(Datasrc* src, nc_type nctype)
{
    Constant* ci = srcpeek(src);
    if(ci != NULL && ci->nctype == nctype) return 1;
    return 0;
}

/**************************************************/

Datasrc
makedatasrc(Datalist* list)
{
    Datasrc src;
    if(list == NULL)
    assert(list != NULL);
    src.data = list->data;
    src.stack = NULL;
    src.autopop = 0;
#ifdef DSPTR
    src.next = src.data;
    src.last = src.data + list->length;
#else
    src.index = 0;
    src.length = list->length;
#endif
    DUMPSRC(&src,"#");
    return src;
}

#ifdef DSPTR
Constant*
srcpeek(Datasrc* ds)
{
    return (ds->next >= ds->last?NULL:ds->next);
}

Constant*
srcnext(Datasrc* ds)
{
    Constant* nxt;
    DUMPSRC(ds,"!");
    nxt = (ds->next >= ds->last?NULL:ds->next++;
    if(ds->autopop) srcpop(ds);
    return nxt;
}

int
srcmore(Datasrc* ds)
{
    return (ds->index < ds->last);
}

static void
srcpushlist0(Datasrc* src, Datalist* cmpd)
{
    Datasrc* ds = (Datasrc*)emalloc(sizeof(Datasrc));
    assert(cmpd != NULL);
    *ds = *src; /* we are going to swap src and ds in effect*/
    src->stack = ds;
    src->data = cmpd->data;
    src->next = src->data;
    src->last = src->next + cmpd->length;
}

void
srcpush(Datasrc* ds)
{
    Constant* con;
    con = (ds->next >= ds->last?NULL:ds->next++;
    if(ds->autopop) srcpop(ds);
    if(con->nctype == NC_COMPOUND) {
        srcpushlist0(ds,con->value.compoundv);
    }
    DUMPSRC(ds,">");
}

#else

Constant*
srcpeek(Datasrc* ds)
{
    return (ds->index >= ds->length?NULL:&ds->data[ds->index]);
}

Constant*
srcnext(Datasrc* ds)
{
    Constant* nxt;
    DUMPSRC(ds,"!");
    nxt = (ds->index >= ds->length?NULL:&ds->data[ds->index++]);
    if(ds->autopop) srcpop(ds);
    return nxt;
}

int
srcmore(Datasrc* ds)
{
    return (ds->index < ds->length);
}

static void
srcpushlist0(Datasrc* src, Datalist* cmpd)
{
    Datasrc* ds = (Datasrc*)emalloc(sizeof(Datasrc));
    assert(cmpd != NULL);
    *ds = *src; /* we are going to swap src and ds in effect*/
    src->stack = ds;
    src->data = cmpd->data;
    src->index = 0;
    src->length = cmpd->length;
}

void
srcpush(Datasrc* ds)
{
    Constant* con;
    con = (ds->index >= ds->length?NULL:ds->data+(ds->index++));
    if(ds->autopop) srcpop(ds);
    if(con->nctype == NC_COMPOUND) {
        srcpushlist0(ds,con->value.compoundv);
    }
    DUMPSRC(ds,">");
}

#endif /*!DSPTR*/

void
srcpushlist(Datasrc* src, Datalist* cmpd)
{
    srcpushlist0(src,cmpd);
    DUMPSRC(src,">!");
}

int
srcpop(Datasrc* src)
{
    Datasrc* prev = src->stack;
    DUMPSRC(src,"<");
    if(prev == NULL) return 0; /* nothing to pop*/
    *src=*prev;
    free(prev);
    return 1;
}

int
srcline(Datasrc* ds)
{
    int index = ds->index;
    int len = ds->length;
    /* pick closest available entry*/
    if(len == 0) return 0;
    if(index >= len) index = len-1;
    return ds->data[ds->index].lineno;
}

void
srcautopop(Datasrc* ds)
{
    ds->autopop = 1;
}

/**************************************************/
#ifdef DEBUG
void
report(char* lead, Datalist* list)
{
extern void bufdump(Datalist*,Bytebuffer*);
Bytebuffer* buf = bbNew();
bufdump(list,buf);
fprintf(stderr,"\n%s::%s\n",lead,bbContents(buf));
fflush(stderr);
bbFree(buf);
}

void
report0(char* lead, Datasrc* src, int index)
{
if(debug == 0) return;
fprintf(stderr,"%s src ",lead);
if(index >=0 ) fprintf(stderr,"(%d)",index);
fprintf(stderr,":: ");
dumpdatasrc(src);
fprintf(stderr,"\n");
fflush(stderr);
}
#endif

/**************************************************/
/* Our goal is to collect chunks of characters (from char or string)*/
/* and convert to a single string of the right size with the*/
/* right padding*/

Constant
gen_string(unsigned long chunksize, Datasrc* src, Bytebuffer* buf)
{
    Constant result;
    Bytebuffer* chunkbuf = buf;
    if(chunkbuf == NULL) chunkbuf = bbNew();
    gen_stringchunks(chunksize,src,chunkbuf);
    result.nctype = NC_STRING;
    result.lineno = srcline(src);
    result.value.stringv.len = bbLength(chunkbuf);
    result.value.stringv.stringv = bbDup(chunkbuf);
    if(buf == NULL) bbFree(chunkbuf);
    return result;
}

static void
gen_stringchunks(unsigned long chunksize, Datasrc* src, Bytebuffer* buf)
{
    Constant* con;

    con = srcnext(src);
    if(con == NULL) {
	/* do nothing*/
    } else if(con->nctype == NC_CHAR) {
	bbAppend(buf,con->value.charv);		
    } else if(con->nctype == NC_STRING) {
	unsigned long len = con->value.stringv.len;
	if(chunksize == 0) {
	    bbAppendn(buf,con->value.stringv.stringv,len);
	} else if(len <= chunksize) {
	    bbAppendn(buf,con->value.stringv.stringv,len);
	    while(bbLength(buf) % chunksize != 0) bbAppend(buf,NC_FILL_CHAR);
	} else { /* (len > chunksize)*/
	    Datalist* dl;
	    Constant str;
	    unsigned long rem;
	    bbAppendn(buf,con->value.stringv.stringv,chunksize);
	    str = *con;
	    str.value.stringv.stringv = strdup(str.value.stringv.stringv);
	    /* remove the initial chunk*/
	    rem = (len - chunksize);
	    str.value.stringv.len = rem;
	    memcpy(str.value.stringv.stringv,str.value.stringv.stringv+chunksize,rem);
	    str.value.stringv.stringv[rem] = '\0';
	    dl = builddatalist(1);
	    dlappend(dl,&str);
	    srcpushlist(src,dl);
	    srcautopop(src);
	}	
    } else if(con->nctype == NC_COMPOUND) {
	semerror(srcline(src),"gen_string: expected string/char constant, found {...}");
    } else {
	semerror(srcline(src),"gen_string: expected string/char constant, found: %d",con->nctype);
    }
}

/* Shallow constant cloning*/
Constant
cloneconstant(Constant* con)
{
    Constant newcon = *con;
    char* s;
    switch (newcon.nctype) {
    case NC_STRING:
	s = (char*)emalloc(newcon.value.stringv.len+1);
	memcpy(s,newcon.value.stringv.stringv,newcon.value.stringv.len);
	s[newcon.value.stringv.len] = '\0';
	newcon.value.stringv.stringv = s;
	break;
    case NC_OPAQUE:
	s = (char*)emalloc(newcon.value.opaquev.len+1);
	memcpy(s,newcon.value.opaquev.stringv,newcon.value.opaquev.len);
	s[newcon.value.opaquev.len] = '\0';
	newcon.value.opaquev.stringv = s;
	break;
    default: break;
    }
    return newcon;
}
