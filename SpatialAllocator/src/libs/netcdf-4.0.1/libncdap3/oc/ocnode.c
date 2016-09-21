/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include "ocdebug.h"

static const unsigned int MAX_UINT = 0xffffffff;

static int mergedas1(OCnode* dds, OCnode* das);
static void* linearize(OCtype etype, OClist* avset);
static int converttype(OCtype etype, char* value, char* memory);
static char* pathtostring(OClist* path, char* separator, int usecdfname);
static void computefullname(OCnode* node);

    /* Process ocnodes to fix various semantic issues*/
void
computeocsemantics(OClist* ocnodes)
{
    unsigned int i;
    OCASSERT((ocnodes != NULL));
    for(i=0;i<oclistlength(ocnodes);i++) {
	OCnode* node = (OCnode*)oclistget(ocnodes,i);
	/* set the container for dims*/
	if(node->octype == OC_Dimension && node->dim.array != NULL) {
	    node->container = node->dim.array->container;
	}
    }
}

void
computefullnames(OCnode* root)
{
    unsigned int i;
    if(root->name != NULL) computefullname(root);
    if(root->subnodes != NULL) { /* recurse*/
        for(i=0;i<oclistlength(root->subnodes);i++) {
	    OCnode* node = (OCnode*)oclistget(root->subnodes,i);
	    computefullnames(node);
	}
    }
}

static void
computefullname(OCnode* node)
{
    char* tmp;
    char* fullname;
    OClist* path;

    OCASSERT((node->name != NULL));
    path = oclistnew();
    collectpathtonode(node,path);
    tmp = pathtostring(path,PATHSEPARATOR,1);
    if(tmp == NULL) {
        fullname = strdup(node->name);
    } else {
        fullname = tmp;
    }
    node->fullname = fullname;
    oclistfree(path);
}

/* Convert path to a string; leave off the dataset name*/
static char*
pathtostring(OClist* path, char* separator, int usecdfname)
{
    int slen,i,len;
    char* pathname;
    if(path == NULL || (len = oclistlength(path))==0) return NULL;
    for(slen=0,i=0;i<len;i++) {
	OCnode* node = (OCnode*)oclistget(path,i);
	if(node->container == NULL || node->name == NULL) continue;
	slen += strlen(node->name);
    }
    slen += ((len-1)*strlen(separator));
    slen += 1;   /* for null terminator*/
    pathname = (char*)ocmalloc(slen);
    MEMCHECK(pathname,NULL);
    pathname[0] = '\0';
    for(i=0;i<len;i++) {
	OCnode* node = (OCnode*)oclistget(path,i);
	if(node->container == NULL || node->name == NULL) continue;
	if(strlen(pathname) > 0) strcat(pathname,separator);
        strcat(pathname,node->name);
    }
    return pathname;
}

/* Collect the set of nodes ending in "node"*/
void
collectpathtonode(OCnode* node, OClist* path)
{
    if(node == NULL) return;
    collectpathtonode(node->container,path);
    oclistpush(path,(ocelem)node);
}

OCnode*
makeocnode(char* name, OCtype ptype, OCconnection* conn)
{
    OCnode* cdf = (OCnode*)ocmalloc(sizeof(OCnode));
    MEMCHECK(cdf,(OCnode*)NULL);
    memset((void*)cdf,0,sizeof(OCnode));
    cdf->name = (name?strdup(name):NULL);
    cdf->octype = ptype;
/*    cdf->subnodes = oclistnew();*/
/*    cdf->attributes = oclistnew();*/
    cdf->array.dimensions = NULL;
    cdf->owner = conn;
    return cdf;
}

Attribute*
makeattribute(char* name, OCtype ptype, OClist* values)
{
    Attribute* att = (Attribute*)ocmalloc(sizeof(Attribute)); /* ocmalloc zeros*/
    MEMCHECK(att,(Attribute*)NULL);
    att->name = nulldup(name);
    att->etype = ptype;
    att->nvalues = oclistlength(values);
    att->values = linearize(att->etype,values);
    return att;
}

static void
marklostattribute(OCnode* att)
{
    oclog(LOGWARN,"Lost attribute: %s",att->name);
}

static void*
linearize(OCtype etype, OClist* avset)
{
    int i;
    int count = oclistlength(avset);
    size_t typesize = octypesize(etype);
    char* memp;
    char* memory = (char*)ocmalloc(count*typesize);
    MEMCHECK(memory,NULL);
    memp = memory;
    for(i=0;i<count;i++) {
	char* value = (char*)oclistget(avset,i);
        if(!converttype(etype,value,memp))
	    OCPANIC1("converttype failure: %d",etype);
	memp += typesize;
    }
    return memory;
}

static int
converttype(OCtype etype, char* value, char* memory)
{
    long iv;
    unsigned long uiv;
    double dv;
    char c[1];
    long long llv;
    unsigned long long ullv;

    switch (etype) {
    case OC_Char:
	if(sscanf(value,"%c",c) != 1) return 0;
	*((char*)memory) = c[0];
	break;
    case OC_Byte:
	if(sscanf(value,"%ld",&iv) != 1) return 0;
        else if(iv > OC_BYTE_MAX || iv < OC_BYTE_MIN) return 0;
	*((signed char*)memory) = (signed char)iv;
	break;
    case OC_UByte:
	if(sscanf(value,"%lu",&uiv) != 1) return 0;
        else if(uiv > OC_UBYTE_MAX) return 0;
	*((unsigned char*)memory) = (unsigned char)uiv;
	break;
    case OC_Int16:
	if(sscanf(value,"%ld",&iv) != 1) return 0;
        else if(iv > OC_INT16_MAX || iv < OC_INT16_MIN) return 0;
	*((signed short*)memory) = (signed short)iv;
	break;
    case OC_UInt16:
	if(sscanf(value,"%ld",&uiv) != 1) return 0;
        else if(uiv > OC_UINT16_MAX) return 0;
	*((unsigned short*)memory) = (unsigned short)uiv;
	break;
    case OC_Int32:
	if(sscanf(value,"%ld",&iv) != 1) return 0;
        else if(iv > OC_INT32_MAX || iv < OC_INT32_MIN) return 0;
	*((signed int*)memory) = (signed int)iv;
	break;
    case OC_UInt32:
	if(sscanf(value,"%ld",&uiv) != 1) return 0;
        else if(uiv > OC_UINT32_MAX) return 0;
	*((unsigned char*)memory) = (unsigned int)uiv;
	break;
    case OC_Int64:
	if(sscanf(value,"%lld",&llv) != 1) return 0;
        /*else if(iv > OC_INT64_MAX || iv < OC_INT64_MIN) return 0;*/
	*((signed long long*)memory) = (signed long long)iv;
	break;
    case OC_UInt64:
	if(sscanf(value,"%llu",&ullv) != 1) return 0;
	*((unsigned long long*)memory) = (unsigned long long)ullv;
	break;
    case OC_Float32:
	if(sscanf(value,"%lf",&dv) != 1) return 0;
	*((float*)memory) = (float)dv;
	break;
    case OC_Float64:
	if(sscanf(value,"%lf",&dv) != 1) return 0;
	*((double*)memory) = (double)dv;
	break;
    case OC_String: case OC_URL:
	*((char**)memory) = strdup(value);
	break;
    default: return 0;
    }
    return 1;
}

/* For those nodes that are uniform in size, compute size
   size of the node*/
size_t
ocsetsize(OCnode* node)
{
    size_t count, subnodesum;
    unsigned int i;
    int isscalar = (node->array.rank == 0);
    size_t instancesize;
    size_t dimsize; /* to give to parent*/

    instancesize = 0; /* assume not uniform*/
    dimsize = 0;

    /* compute total # of elements if dimensioned*/
    count = 1;
    for(i=0;i<node->array.rank;i++) {
	OCnode* dim = (OCnode*)oclistget(node->array.dimensions,i);
	count *= (dim->dim.declsize);
    }

    /* Recursively compute sizes of subnodes, if any*/
    subnodesum = 0;
    if(node->subnodes != NULL) {
	int nonuniform = 0;
        for(i=0;i<oclistlength(node->subnodes);i++) {
	    OCnode* subnode = (OCnode*)oclistget(node->subnodes,i);
	    size_t subsize = ocsetsize(subnode); /* includes subnode dimension counts*/
	    if(subsize == 0) nonuniform = 1;
	    subnodesum += subsize;
	}
	if(nonuniform) subnodesum = 0;
    }

    switch (node->octype) {
        case OC_Primitive:
	    switch (node->etype) {
	    case OC_String: case OC_URL: /* not uniform*/
		instancesize = 0;
		dimsize = 0; /* not uniform*/
		break; /* not uniform*/
	    case OC_Byte:
	    case OC_UByte:
	    case OC_Char:
		instancesize = (isscalar?BYTES_PER_XDR_UNIT:1);
	        dimsize = instancesize;
		/* We have to watch out for the fact that packed instances have padding in the xdr packet*/
		if(!isscalar) { /* padding to multiple of BYTE_PER_XDR_UNIT*/
		    unsigned int rem;
	            dimsize = count*instancesize;
		    rem = (dimsize % BYTES_PER_XDR_UNIT);
		    if(rem > 0) dimsize += (BYTES_PER_XDR_UNIT - rem);
		    dimsize += 2*BYTES_PER_XDR_UNIT; /* the dimension counts (repeated)*/
		}
		break;
	    case OC_Float64:
	    case OC_Int64:
	    case OC_UInt64:
		instancesize = (2*BYTES_PER_XDR_UNIT); /*double = 2 xdr units*/
	        dimsize = count*instancesize + (isscalar?0:2*BYTES_PER_XDR_UNIT);
		break;
	    default:
		instancesize = (BYTES_PER_XDR_UNIT); /* all others: 1 xdr unit*/
	        dimsize = count*instancesize + (isscalar?0:2*BYTES_PER_XDR_UNIT);
		break;
	    }
	    break;

        case OC_Sequence: /* never uniform, but instances may be*/
	    dimsize = 0;
	    instancesize = subnodesum;
	    break;

        case OC_Grid:
	case OC_Dataset:
        case OC_Structure:
	    instancesize = subnodesum;
	    dimsize = count*instancesize + (isscalar?0:BYTES_PER_XDR_UNIT);
	    break;

        default: OCPANIC1("ocmap: encountered unexpected node type: %x",node->octype);
	    break;
    }
    node->xdrinstancesize = instancesize;
    node->xdrdimsize = dimsize;
    return dimsize;
}

void
ocfreenodes(OClist* nodes)
{
    unsigned int i,j;
    for(i=0;i<oclistlength(nodes);i++) {
	OCnode* node = (OCnode*)oclistget(nodes,i);
        ocfree(node->name);
        ocfree(node->fullname);
        while(oclistlength(node->att.values) > 0) {
	    char* value = (char*)oclistpop(node->att.values);
	    ocfree(value);
        }
        while(oclistlength(node->attributes) > 0) {
            Attribute* attr = (Attribute*)oclistpop(node->attributes);
	    ocfree(attr->name);
	    /* If the attribute type is string, then we need to free them*/
	    if(attr->etype == OC_String || attr->etype == OC_URL) {
		char** strings = (char**)attr->values;
		for(j=0;j<attr->nvalues;j++) {ocfree(*strings); strings++;}
	    }
	    ocfree(attr->values);
	    ocfree(attr);
        }
        if(node->array.dimensions != NULL) oclistfree(node->array.dimensions);
        if(node->subnodes != NULL) oclistfree(node->subnodes);
        if(node->att.values != NULL) oclistfree(node->att.values);
        if(node->attributes != NULL) oclistfree(node->attributes);
        ocfree(node);
    }
}

/*
In order to be as compatible as possible with libdap,
we use the same algorithm for DAS->DDS matching.
As described there, the algorithm is as follows.
    If the [attribute] name contains one or
    more field separators then look for a [DDS]variable whose
    name matches exactly. If the name contains no field separators then
    the look first in the top level [of the DDS] and then in all subsequent
    levels and return the first occurrence found. In general, this
    searches constructor types in the order in which they appear
    in the DDS, but there is no requirement that it do so.

    Note: If a dataset contains two constructor types which have field names
    that are the same (say point.x and pair.x) one should use fully qualified
    names to get each of those variables.
*/

int
oc_ddsdasmerge(OCconnection* state, int whichdds)
{
    OClist* dasglobals = oclistnew();
    OClist* dasnodes = oclistnew();
    OClist* varnodes = oclistnew();
    OClist* ddsnodes;
    unsigned int i,j;

    /* 1. Determine what (or if) to merge*/
    if(state->schema.das.tree == NULL) return THROW(OC_EINVAL);  /* no DAS to merge*/
    if(whichdds == MERGEDDS
	&& state->schema.dds.tree == NULL) return THROW(OC_EINVAL); /* no DDS to merge;*/
    else if(whichdds == MERGEDATADDS
	    && state->schema.datadds.tree == NULL) return THROW(OC_EINVAL); /* no DDS to merge;*/

    ddsnodes = (whichdds == MERGEDDS?state->schema.dds.nodes:state->schema.datadds.nodes);

    /* 1. collect all the relevant DAS nodes;
          namely those that contain at least one
          attribute value.
          Simultaneously look for potential ambiguities
          if found; complain but continue: result are indeterminate.
          also collect globals separately*/
    for(i=0;i<oclistlength(state->schema.das.nodes);i++) {
	OCnode* das = (OCnode*)oclistget(state->schema.das.nodes,i);
	int hasattributes = 0;
	if(das->octype == OC_Attribute) continue; /* ignore these for now*/
	if(das->att.isglobal) {oclistpush(dasglobals,(ocelem)das); continue;}
	for(j=0;j<oclistlength(das->subnodes);j++) {
	    OCnode* subnode = (OCnode*)oclistget(das->subnodes,j);
	    if(subnode->octype == OC_Attribute) {hasattributes = 1; break;}
	}
	if(hasattributes) {
	    /* Look for previously collected nodes with same name*/
            for(j=0;j<oclistlength(dasnodes);j++) {
	        OCnode* das2 = (OCnode*)oclistget(dasnodes,j);
		if(strcmp(das->name,das2->name)==0) {
		    oclog(LOGWARN,"oc_mergedas: potentially ambiguous DAS name: %s",das->name);
		}
	    }
	    oclistpush(dasnodes,(ocelem)das);
	}
    }

    /* 2. collect all the leaf DDS nodes (of type OC_Primitive)*/
    for(i=0;i<oclistlength(ddsnodes);i++) {
	OCnode* dds = (OCnode*)oclistget(ddsnodes,i);
	if(dds->octype == OC_Primitive) oclistpush(varnodes,(ocelem)dds);
    }

    /* 3. For each das node, locate matching DDS node and attach
          attributes to the DDS node.
          Match means:
          1. DAS->fullname :: DDS->fullname
          2. DAS->name :: DDS->fullname (support DAS names with embedded '.'
          3. DAS->name :: DDS->name
          walk backward to simplify removal semantics*/
    for(i=oclistlength(dasnodes);i>0;i--) {
	OCnode* das = (OCnode*)oclistget(dasnodes,i-1);
        for(j=0;j<oclistlength(varnodes);j++) {
	    OCnode* dds = (OCnode*)oclistget(varnodes,j);
	    if(strcmp(das->fullname,dds->fullname)==0
	       || strcmp(das->name,dds->fullname)==0
	       || strcmp(das->name,dds->name)==0) {
		mergedas1(dds,das);
		oclistremove(dasnodes,i-1); /* only assign once*/
		break;
	    }
	}
    }

    /* 4. If there are attributes left, then complain about them being lost.*/
    for(i=0;i<oclistlength(dasnodes);i++) {
	OCnode* das = (OCnode*)oclistget(dasnodes,i);
	marklostattribute(das);
    }

    /* 5. Assign globals*/
    for(i=0;i<oclistlength(dasglobals);i++) {
	OCnode* das = (OCnode*)oclistget(dasnodes,i);
	OCnode* root = (whichdds == MERGEDDS?state->schema.dds.tree:state->schema.datadds.tree);
	mergedas1(root,das);
    }
    /* cleanup*/
    oclistfree(dasglobals);
    oclistfree(dasnodes);
    oclistfree(varnodes);
    return THROW(OC_NOERR);
}

static int
mergedas1(OCnode* dds, OCnode* das)
{
    unsigned int i;
    int stat = OC_NOERR;
    if(das == NULL) return OC_NOERR; /* nothing to do */
    if(dds->attributes == NULL) dds->attributes = oclistnew();
    /* assign the simple attributes in the das set to this dds node*/
    for(i=0;i<oclistlength(das->subnodes);i++) {
	OCnode* attnode = (OCnode*)oclistget(das->subnodes,i);
	if(attnode->octype == OC_Attribute) {
	    Attribute* att = makeattribute(attnode->name,
						attnode->etype,
						attnode->att.values);
            oclistpush(dds->attributes,(ocelem)att);
	}
    }
    return THROW(stat);
}


#ifdef IGNORE

int
oc_ddsdasmerge(OCconnection* state, OCnode* ddsroot, OCnode* dasroot)
{
    int i,j;
    int stat = OC_NOERR;
    OClist* globals = oclistnew();
    if(dasroot == NULL) return THROW(stat);
    /* Start by looking for global attributes*/
    for(i=0;i<oclistlength(dasroot->subnodes);i++) {
	OCnode* node = (OCnode*)oclistget(dasroot->subnodes,i);
	if(node->att.isglobal) {
	    for(j=0;j<oclistlength(node->subnodes);j++) {
		OCnode* attnode = (OCnode*)oclistget(node->subnodes,j);
		Attribute* att = makeattribute(attnode->name,
						attnode->etype,
						attnode->att.values);
		oclistpush(globals,(ocelem)att);
	    }
	}
    }
    ddsroot->attributes = globals;
    /* Now try to match subnode names with attribute set names*/
    for(i=0;i<oclistlength(dasroot->subnodes);i++) {
	OCnode* das = (OCnode*)oclistget(dasroot->subnodes,i);
	int match = 0;
        if(das->att.isglobal) continue;
        if(das->octype == OC_Attributeset) {
            for(j=0;j<oclistlength(ddsroot->subnodes) && !match;j++) {
	        OCnode* dds = (OCnode*)oclistget(ddsroot->subnodes,j);
	        if(strcmp(das->name,dds->name) == 0) {
		    match = 1;
	            stat = mergedas1(dds,das);
	            if(stat != OC_NOERR) break;
		}
	    }
	}
        if(!match) {marklostattribute(das);}
    }
    if(stat == OC_NOERR) ddsroot->attributed = 1;
    return THROW(stat);
}

/* Merge das attributes into the dds node*/

static int
mergedas1(OCnode* dds, OCnode* das)
{
    int i,j;
    int stat = OC_NOERR;
    if(dds->attributes == NULL) dds->attributes = oclistnew();
    /* assign the simple attributes in the das set to this dds node*/
    for(i=0;i<oclistlength(das->subnodes);i++) {
	OCnode* attnode = (OCnode*)oclistget(das->subnodes,i);
	if(attnode->octype == OC_Attribute) {
	    Attribute* att = makeattribute(attnode->name,
						attnode->etype,
						attnode->att.values);
            oclistpush(dds->attributes,(ocelem)att);
	}
    }
    /* Try to merge any enclosed sets with subnodes of dds*/
    for(i=0;i<oclistlength(das->subnodes);i++) {
	OCnode* dasnode = (OCnode*)oclistget(das->subnodes,i);
	int match = 0;
        if(dasnode->octype == OC_Attribute) continue; /* already dealt with above*/
        for(j=0;j<oclistlength(dds->subnodes) && !match;j++) {
	    OCnode* ddsnode = (OCnode*)oclistget(dds->subnodes,j);
	    if(strcmp(dasnode->name,ddsnode->name) == 0) {
	        match = 1;
	        stat = mergedas1(ddsnode,dasnode);
	        if(stat != OC_NOERR) break;
	    }
	}
        if(!match) {marklostattribute(dasnode);}
    }
    return THROW(stat);
}
#endif
