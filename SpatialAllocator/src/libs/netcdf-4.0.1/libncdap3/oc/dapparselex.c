/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#define URLCVT

#include "ocinternal.h"
#include "ocdebug.h"
#include "dapparselex.h"
#ifdef USE_DAP
#include "daptab.h"
#else
#include "dap.tab.h"
#endif

#if 0
/* I removed this because it was breaking the link on OS/X with bison
   2.3. Removing this does not affect the linux build, so I'm going to
   make the change (and in ceparselex.c). jhrg 2/27/09 */
int dapdebug; /*  must be global. */
#endif

extern int dapparse(DAPparsestate*);

/* Forward */
static void dap_parse_cleanup(DAPparsestate*);
static void addedges(OCnode* node);
static int isglobalname(char* name);
static void lexcleanup(Lexstate*);
static void lexinit(char* input,Lexstate*);
static void dumptoken(Lexstate* lexstate);
static OCnode* newocnode(char* name, OCtype octype, DAPparsestate* state);

/****************************************************/

Object
datasetbody(DAPparsestate* state, Object name, Object decls)
{
    OCnode* node = newocnode((char*)name,OC_Dataset,state);
    node->subnodes = (OClist*)decls;
    addedges(node);
    state->result = node;
    return node;
}

Object
attributebody(DAPparsestate* state, Object attrlist)
{
    OCnode* node = newocnode(NULL,OC_Attributeset,state);
    node->subnodes = (OClist*)attrlist;
    addedges(node);
    state->result = node;
    return node;
}

Object
declarations(DAPparsestate* state, Object decls, Object decl)
{
    OClist* alist = (OClist*)decls;
    if(alist == NULL)
	 alist = oclistnew();
    else
	oclistpush(alist,(ocelem)decl);
    return alist;
}

Object
arraydecls(DAPparsestate* state, Object arraydecls, Object arraydecl)
{
    OClist* alist = (OClist*)arraydecls;
    if(alist == NULL)
	alist = oclistnew();
    else
	oclistpush(alist,(ocelem)arraydecl);
    return alist;
}

static int
check_int32(char* val, long* value)
{
    char* ptr;
    int ok = 1;
    long iv = strtol(val,&ptr,0); /* 0=>auto determine base */
    if((iv == 0 && val == ptr) || *ptr != '\0') {ok=0; iv=1;}
    else if(iv > OC_INT32_MAX || iv < OC_INT32_MIN) ok=0;
    if(value != NULL) *value = iv;
    return ok;
}

Object
arraydecl(DAPparsestate* state, Object name, Object size)
{
    long value;
    OCnode* dim;
    if(!check_int32(size,&value))
	dap_parse_error(state,"Dimension not an integer");
    if(name != NULL)
	dim = newocnode((char*)name,OC_Dimension,state);
    else
	dim = newocnode(NULL,OC_Dimension,state);
    dim->dim.declsize = value;
    return dim;
}

static int
duplicateattributes(OClist* alist)
{
    unsigned int i;
    for(i=0;i<oclistlength(alist);i++) {
	unsigned int j;
	OCnode* attri = (OCnode*)oclistget(alist,i);
        for(j=i+1;j<oclistlength(alist);j++) {
	    OCnode* attrj = (OCnode*)oclistget(alist,j);
	    if(strcmp(attri->name,attrj->name)==0)
		return 1;
	}
    }
    return 0;
}


Object
attrlist(DAPparsestate* state, Object attrlist, Object attrtuple)
{
    OClist* alist = (OClist*)attrlist;
    if(alist == NULL)
	alist = oclistnew();
    else {
        oclistpush(alist,(ocelem)attrtuple);
        if(duplicateattributes(alist))
	    dap_parse_error(state,"Duplicate attribute names at same level");
    }
    return alist;
}

Object
attrvalue(DAPparsestate* state, Object valuelist, Object value, Object etype)
{
    OClist* alist = (OClist*)valuelist;
    if(alist == NULL) alist = oclistnew();
    oclistpush(alist,(ocelem)strdup(value));
    return alist;
}

Object
attribute(DAPparsestate* state, Object name, Object values, Object etype)
{
    OCnode* att;
    att = newocnode((char*)name,OC_Attribute,state);
    att->etype = (OCtype)etype;
    att->att.values = (OClist*)values;
    return att;
}

Object
attrset(DAPparsestate* state, Object name, Object attributes)
{
    OCnode* attset;
    attset = newocnode((char*)name,OC_Attributeset,state);
    /* Check var set vs global set */
    attset->att.isglobal = isglobalname(name);
    attset->subnodes = (OClist*)attributes;
    return attset;
}

static int
isglobalname(char* name)
{
    int len = strlen(name);
    int glen = strlen("global");
    char* p;
    if(len < glen) return 0;
    p = name + (len - glen);
    if(strcasecmp(p,"global") != 0)
	return 0;
    return 1;
}

#ifdef IGNORE
static int
isnumber(const char* text)
{
    for(;*text;text++) {if(!isdigit(*text)) return 0;}
    return 1;
}
#endif

static void
dimension(OCnode* node, OClist* dimensions)
{
    unsigned int i;
    unsigned int rank = oclistlength(dimensions);
    node->array.dimensions = (OClist*)dimensions;
    node->array.rank = rank;
    for(i=0;i<rank;i++) {
        OCnode* dim = (OCnode*)oclistget(node->array.dimensions,i);
        dim->dim.array = node;
	dim->dim.arrayindex = i;
#ifdef IGNORE
	if(dim->name == NULL) {
	    dim->dim.anonymous = 1;
	    dim->name = dimnameanon(node->name,i);
	}
#endif
    }
}

char*
dimnameanon(char* basename, unsigned int index)
{
    char name[64];
    sprintf(name,"%s_%d",basename,index);
    return strdup(name);
}

Object
makebase(DAPparsestate* state, Object name, Object etype, Object dimensions)
{
    OCnode* node;
    node = newocnode((char*)name,OC_Primitive,state);
    node->etype = (OCtype)etype;
    dimension(node,(OClist*)dimensions);
    return node;
}

Object
makestructure(DAPparsestate* state, Object name, Object dimensions, Object fields)
{
    OCnode* node;
    node = newocnode(name,OC_Structure,state);
    node->subnodes = fields;
    dimension(node,(OClist*)dimensions);
    addedges(node);
    return node;
}

Object
makesequence(DAPparsestate* state, Object name, Object members)
{
    OCnode* node;
    node = newocnode(name,OC_Sequence,state);
    node->subnodes = members;
    addedges(node);
    return node;
}

Object
makegrid(DAPparsestate* state, Object name, Object arraydecl, Object mapdecls)
{
    OCnode* node;
    node = newocnode(name,OC_Grid,state);
    node->subnodes = mapdecls;
    oclistinsert(node->subnodes,0,(ocelem)arraydecl);
    addedges(node);
    return node;
}

static void
addedges(OCnode* node)
{
    unsigned int i;
    if(node->subnodes == NULL) return;
    for(i=0;i<oclistlength(node->subnodes);i++) {
        OCnode* subnode = (OCnode*)oclistget(node->subnodes,i);
	subnode->container = node;
    }
}

int
daperror(DAPparsestate* state, const char* msg)
{
    dap_parse_error(state,msg);
    return 0;
}

static char*
flatten(char* s, char* tmp, int tlen)
{
    int c;
    char* p,*q;
    snprintf(tmp,tlen,"%s",s);
    tmp[tlen-1] = '\0';
    p = (q = tmp);
    while((c=*p++)) {
	switch (c) {
	case '\r': case '\n': break;
	case '\t': *q++ = ' '; break;
	case ' ': if(*p != ' ') *q++ = c; break;
	default: *q++ = c;
	}
    }
    *q = '\0';
    return tmp;
}

void
dap_parse_error(DAPparsestate* state, const char *fmt, ...)
{
    va_list argv;
    char tmp[64];
    va_start(argv,fmt);
    (void) vfprintf(stderr,fmt,argv) ;
    (void) fputc('\n',stderr) ;
    (void) fprintf(stderr,"context: ^%s",flatten(state->lexstate.next,tmp,sizeof(tmp)));
    (void) fflush(stderr);	/* to ensure log files are current */
}


int ddsdebug=0;

static DAPparsestate*
dap_parse_init(char* buf)
{
    DAPparsestate* state = (DAPparsestate*)ocmalloc(sizeof(DAPparsestate)); /*ocmalloc zeros*/
    MEMCHECK(state,(DAPparsestate*)NULL);
    if(buf==NULL) {
        dap_parse_error(state,"dap_parse_init: no input buffer");
        dap_parse_cleanup(state);
	return NULL;
    }
    lexinit(buf,&state->lexstate);
    return state;
}

static void
dap_parse_cleanup(DAPparsestate* state)
{
    lexcleanup(&state->lexstate);
    free(state);
}

/* Create an ocnode and capture in the state->ocnode list */
static OCnode*
newocnode(char* name, OCtype octype, DAPparsestate* state)
{
    OCnode* node = makeocnode(name,octype,state->conn);
    oclistpush(state->ocnodes,(ocelem)node);
    return node;
}

/* Wrapper for dapparse */
OCnode*
DAPparse(OCconnection* conn, char* packet, OClist* cdfnodes)
{
    OCnode* cdf = NULL;
    DAPparsestate* state = dap_parse_init(packet);
    state->ocnodes = cdfnodes;
    state->conn = conn;
    if(ocdebug >= 2) dapdebug = 1;
    if(dapparse(state) == 0) {
	cdf = state->result;
    }
    dap_parse_cleanup(state);
    return cdf;
}

/*****************************************/
/*
Simple lexer
*/
static void
lexinit(char* input, Lexstate* lexstate)
{
    memset((void*)lexstate,0,sizeof(Lexstate));
    lexstate->input = strdup(input);
    lexstate->next = lexstate->input;
    lexstate->yytext = ocbnew();
    lexstate->reclaim = oclistnew();
}

static void
lexcleanup(Lexstate* lexstate)
{
    unsigned int i;
    for(i=0;i<oclistlength(lexstate->reclaim);i++)
	ocfree((void*)oclistget(lexstate->reclaim,i));
    oclistfree(lexstate->reclaim);
    if(lexstate->input != NULL) ocfree(lexstate->input);
    if(lexstate->yytext != NULL) ocbfree(lexstate->yytext);
    if(lexstate->lasttoken.text != NULL) ocfree(lexstate->lasttoken.text);
}

/* First character in TOKEN_IDENTifier or number */
static char* wordchars1 =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+_/%.\\*";
static char* wordcharsn =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+_/%.\\*#";
/* Legal single char tokens */
static char* single = "{}[]:;=,";
/* Hex digits */
static char hexdigits[] = "0123456789abcdefABCDEF";

static int
tohex(int c)
{
    if(c >= 'a' && c <= 'f') return (c - 'a') + 0xa;
    if(c >= 'A' && c <= 'F') return (c - 'A') + 0xa;
    if(c >= '0' && c <= '9') return (c - '0');
    return -1;
}

#define NKEYWORDS 17

static char* keywords[17] = {
"alias",
"array",
"attributes",
"byte",
"dataset",
"float32",
"float64",
"grid",
"int16",
"int32",
"maps",
"sequence",
"string",
"structure",
"uint16",
"uint32",
"url",
};

static int keytokens[17] = {
SCAN_ALIAS,
SCAN_ARRAY,
SCAN_ATTR,
SCAN_BYTE,
SCAN_DATASET,
SCAN_FLOAT32,
SCAN_FLOAT64,
SCAN_GRID,
SCAN_INT16,
SCAN_INT32,
SCAN_MAPS,
SCAN_SEQUENCE,
SCAN_STRING,
SCAN_STRUCTURE,
SCAN_UINT16,
SCAN_UINT32,
SCAN_URL
};

int
daplex(YYSTYPE *lvalp, DAPparsestate* state)
{
    Lexstate* lexstate = &state->lexstate;
    int token;
    int c;
    unsigned int i;
    char* p=lexstate->next;
    token = 0;
    ocbclear(lexstate->yytext);
    /* invariant: p always points to current char */
    for(p=lexstate->next;token==0&&(c=*p);p++) {
	if(c == '\n') {
	    lexstate->lineno++;
	} else if(c <= ' ' || c == '\177') {
	    /* whitespace: ignore */
	} else if(c == '#') {
	    /* single line comment */
	    while((c=*(++p))) {if(c == '\n') break;}
	} else if(strchr(single,c) != NULL) {
	    /* don't put in lexstate->yytext to avoid memory leak */
	    token = c;
	} else if(c == '"') {
	    int more = 1;
	    /* We have a string token; will be reported as SCAN_WORD */
	    while(more && (c=*(++p))) {
		switch (c) {
		case '"': more=0; break;
		case '\\':
		    c=*(++p);
		    switch (c) {
		    case 'r': c = '\r'; break;
		    case 'n': c = '\n'; break;
		    case 'f': c = '\f'; break;
		    case 't': c = '\t'; break;
		    case 'x': {
			int d1,d2;
			c = '?';
			++p;
		        d1 = tohex(*p++);
			if(d1 < 0) {
			    daperror(state,"Illegal \\xDD in TOKEN_STRING");
			} else {
			    d2 = tohex(*p++);
			    if(d2 < 0) {
			        daperror(state,"Illegal \\xDD in TOKEN_STRING");
			    } else {
				c=(((unsigned int)d1)<<4) | (unsigned int)d2;
			    }
			}
		    } break;
		    default: break;
		    }
		    break;
		default: break;
		}
		if(more) ocbappend(lexstate->yytext,c);
	    }
	    token=SCAN_WORD;
	} else if(strchr(wordchars1,c) != NULL) {
	    /* we have a SCAN_WORD */
	    ocbappend(lexstate->yytext,c);
	    while((c=*(++p))) {
#ifdef URLCVT
		if(c == '%' && p[1] != 0 && p[2] != 0
			    && strchr(hexdigits,p[1]) != NULL
                            && strchr(hexdigits,p[2]) != NULL) {
		    int d1,d2;
		    d1 = tohex(p[1]);
		    d2 = tohex(p[2]);
		    if(d1 >= 0 || d2 >= 0) {
			c=(((unsigned int)d1)<<4) | (unsigned int)d2;
			p+=2;
		    }
		} else {
		    if(strchr(wordcharsn,c) == NULL) {p--; break;}
		}
	        ocbappend(lexstate->yytext,c);
#else

		if(strchr(wordcharsn,c) == NULL) {p--; break;}
	        ocbappend(lexstate->yytext,c);
#endif
	    }
	    token=SCAN_WORD; /* assume */
	    ocbappend(lexstate->yytext,'\0');
	    /* check for keyword */
	    for(i=0;i<NKEYWORDS;i++) {
		if(strcasecmp(keywords[i],ocbcontents(lexstate->yytext))==0) {
		    token=keytokens[i];
	            /* don't put in lexstate->yytext to avoid memory leak */
		    ocbclear(lexstate->yytext);
		    break;
		}
	    }
	} else { /* illegal */
	}
    }
    lexstate->next = p;
    if(lexstate->lasttoken.text != NULL) free(lexstate->lasttoken.text);
    lexstate->lasttoken.text = ocbdup(lexstate->yytext);
    lexstate->lasttoken.token = token;
    if(ocdebug >= 2) dumptoken(lexstate);

    /* Prepare return value */
    /*Put value onto Bison stack*/

    /* Note: this was a bad idea because it sticks malloc'd strings */
    /* on the bison stack which makes it hard to reclaim them. */
    /* Bad (but usable) solution: capture all these strings int */
    /* list in the lexstate and reclaim at end of parse.*/
    if(ocblength(lexstate->yytext) == 0)
        *lvalp = NULL;
    else {
        *lvalp = ocbdup(lexstate->yytext);
	oclistpush(lexstate->reclaim,(ocelem)*lvalp); /* save for reclamation*/
    }

    return token;      /* Return the type of the token.  */
}

static void
dumptoken(Lexstate* lexstate)
{
    char ctoken[4];
    char* stoken;
    switch (lexstate->lasttoken.token) {
    case SCAN_BYTE: stoken = "byte"; break;
    case SCAN_INT16: stoken = "int16"; break;
    case SCAN_UINT16: stoken = "uint16"; break;
    case SCAN_INT32: stoken = "int32"; break;
    case SCAN_UINT32: stoken = "uint32"; break;
    case SCAN_FLOAT32: stoken = "float32"; break;
    case SCAN_FLOAT64: stoken = "float64"; break;
    case SCAN_STRING: stoken = "string"; break;
    case SCAN_URL : stoken = "url"; break;
    case SCAN_DATASET: stoken = "dataset"; break;
    case SCAN_SEQUENCE: stoken = "sequence"; break;
    case SCAN_STRUCTURE: stoken = "structure"; break;
    case SCAN_GRID: stoken = "grid"; break;
    case SCAN_ARRAY: stoken = "array"; break;
    case SCAN_MAPS : stoken = "maps"; break;
    case SCAN_ATTR: stoken = "attr"; break;
    case SCAN_ALIAS : stoken = "alias"; break;
    case SCAN_DATABEGIN: stoken = "databegin"; break;
    default:
	strcpy(ctoken,"'X'");
	ctoken[1] = (char)lexstate->lasttoken.token;
	stoken = ctoken;
    }
    if(lexstate->lasttoken.token == SCAN_WORD) {
        fprintf(stderr,"TOKEN = |%s|\n",lexstate->lasttoken.text);
    } else {
        fprintf(stderr,"TOKEN = %s\n",stoken);
    }
}
