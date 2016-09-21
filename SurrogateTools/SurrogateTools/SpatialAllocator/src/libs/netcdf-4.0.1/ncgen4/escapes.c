/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/ncgen4/escapes.c,v 1.4 2009/03/11 18:26:17 dmh Exp $
 *********************************************************************/

#include "includes.h"

/* Forward*/
static size_t localstrlcat(char*,const char*,size_t);

/*
 * "Expands" valid escape sequences in yystring (read by lex) into the
 * apropriate characters in termstring.  For example, the two character
 * sequence "\t" in yystring would be converted into a single tab character
 * in termstring.  On return, termstring is properly terminated.
 */

void
expand_escapes(
     char *termstring,		/* returned, with escapes expanded */
     char *yytext,
     int yyleng)
{
    char *s, *t, *endp;
    
    yytext[yyleng-1]='\0';	/* don't copy quotes */
    /* expand "\" escapes, e.g. "\t" to tab character  */
    s = termstring;
    t = yytext+1;
    while(*t) {
	if (*t == '\\') {
	    t++;
	    switch (*t) {
	      case 'a':
		*s++ = '\007'; t++; /* will use '\a' when STDC */
		break;
	      case 'b':
		*s++ = '\b'; t++;
		break;
	      case 'f':
		*s++ = '\f'; t++;
		break;
	      case 'n':
		*s++ = '\n'; t++;
		break;
	      case 'r':
		*s++ = '\r'; t++;
		break;
	      case 't':
		*s++ = '\t'; t++;
		break;
	      case 'v':
		*s++ = '\v'; t++;
		break;
	      case '\\':
		*s++ = '\\'; t++;
		break;
	      case '?':
		*s++ = '\177'; t++;
		break;
	      case '\'':
		*s++ = '\''; t++;
		break;
	      case '\"':
		*s++ = '\"'; t++;
		break;
	      case 'x':
		t++; /* now t points to one or more hex digits */
		*s++ = (char) strtol(t, &endp, 16);
		t = endp;
		break;
	      case '0':
	      case '1':
	      case '2':
	      case '3':
	      case '4':
	      case '5':
	      case '6':
	      case '7':
		/* t now points to octal digits */
		*s++ = (char) strtol(t, &endp, 8);
		t = endp;
		break;
	      default:
		*s++ = *t++;
		break;
	    }
	} else {
	    *s++ = *t++;
	}
    }
    *s = '\0';
    return;
}

/*
 * Replace escaped chars in CDL representation of name such as
 * 'abc\:def\ gh\\i' with unescaped version, such as 'abc:def gh\i'.
 */
/* ?? This seems redundant over expand_escapes*/
void
deescapify(char *name)
{
    const char *cp = name;
    char *sp;
    size_t len = strlen(name);
    char *newname;

    if(strchr(name, '\\') == NULL)
	return;

    newname = (char *) emalloc(len + 1);
    cp = name;
    sp = newname;
    while(*cp != '\0') { /* delete '\' chars, except change '\\' to '\' */
	switch (*cp) {
	case '\\':
	    if(*(cp+1) == '\\') {
		*sp++ = '\\';
		cp++;
	    }
	    break;
	default:
	    *sp++ = *cp;
	    break;
	}
	cp++;
    }
    *sp = '\0';
    /* assert(strlen(newname) <= strlen(name)); */
    strncpy(name, newname, len+1); /* watch out for trailing null*/
    free(newname);
    return;
}

/*
Given a character c, fill s with the character suitably escaped.
E.g. c = '\t' => s="\t"
Caller must ensure enough space
Currently does not handle unicode
Returns s as it result.
*/

char*
escapifychar(int c, char* s0, int quote)
{
    char* s = s0;
    if(c == '\\') {
	*s++ = '\\'; *s++='\\';
    } else if(c == quote) {
	*s++ = '\\'; *s++=(char)quote;
    } else if(c >= ' ' && c != '\177') {
	*s++ = (char)c;
    } else {
        switch (c) {
	case '\b': strcpy(s,"\\b"); s+=2; break;
	case '\f': strcpy(s,"\\f"); s+=2; break;
	case '\n': strcpy(s,"\\n"); s+=2; break;
	case '\r': strcpy(s,"\\r"); s+=2; break;
	case '\t': strcpy(s,"\\t"); s+=2; break;
	case '\v': strcpy(s,"\\v"); s+=2; break;
	default: {
	    int oct1 = (c & 007);
	    int oct2 = ((c >> 3) & 007);
	    int oct3 = ((c >> 6) & 003);
	    *s++ = '\\';
	    *s++ = oct3 + '0';
	    *s++ = oct2 + '0';
	    *s++ = oct1 + '0';
	} break;
	}
    }
    *s = '\0';
    return s0;
}

/* Return a pool string that is s0 with all characters*/
/* ecaped that require it.  The resulting string is not*/
/* surrounded by quotes.*/
/* Since the string might actually contain nulls, specify the length.*/

char*
escapify(char* s0, int quote, size_t len)
{
    int i;
    char* result;
    result = poolalloc(1+4*len); /* overkill to support maximal expansion*/
    result[0] = '\0';
    for(i=0;i<len;i++) {
	char tmp[8];
	escapifychar(s0[i],tmp,quote);
        strcat(result,tmp);
    }
    return result;        
}

/*
 * Replace special chars in name so it can be used in C and Fortran
 * variable names without causing syntax errors.  Here we just replace
 * each "-" in a name with "_MINUS_", each "." with "_PERIOD_", etc.
 * For bytes with high bit set, from UTF-8 encoding of Unicode, just
 * replace with "_xHH", where each H is the appropriate hex digit.  If
 * a name begins with a number N, such as "4LFTX", replace with
 * "DIGIT_N_", such as "DIGIT_4_LFTX".
 *
 * It is required that decodify be idempotent:
 * i.e. decodify(decodify(s)) == decodify(s)
 *
 * Returned name is pool alloc'd so is transient
 */
extern char*
decodify (
    const char *name)
{
    int count;		/* number chars in newname */
    char *newname;
    const char *cp;
    char *sp;
    static int init = 0;
    static char* repls[256];	/* replacement string for each char */
    static int lens[256];	/* lengths of replacement strings */
    static struct {
	char c;
	char *s;
    } ctable[] = {
	{' ', "_SPACE_"},
	{'!', "_EXCLAMATION_"},
	{'"', "_QUOTATION_"},
	{'#', "_HASH_"},
	{'$', "_DOLLAR_"},
	{'%', "_PERCENT_"},
	{'&', "_AMPERSAND_"},
	{'\'', "_APOSTROPHE_"},
	{'(', "_LEFTPAREN_"},
	{')', "_RIGHTPAREN_"},
	{'*', "_ASTERISK_"},
	{'+', "_PLUS_"},
	{',', "_COMMA_"},
	{'-', "_MINUS_"},
	{'.', "_PERIOD_"},
	{':', "_COLON_"},
	{';', "_SEMICOLON_"},
	{'<', "_LESSTHAN_"},
	{'=', "_EQUALS_"},
	{'>', "_GREATERTHAN_"},
	{'?', "_QUESTION_"},
	{'@', "_ATSIGN_"},
	{'[', "_LEFTBRACKET_"},
	{'\\', "_BACKSLASH_"},
	{']', "_RIGHTBRACKET_"},
	{'^', "_CIRCUMFLEX_"},
	{'`', "_BACKQUOTE_"},
	{'{', "_LEFTCURLY_"},
	{'|', "_VERTICALBAR_"},
	{'}', "_RIGHTCURLY_"},
	{'~', "_TILDE_"},
 	{'/', "_SLASH_"} 		/* should not occur in names */
/* 	{'_', "_UNDERSCORE_"} */
    };
    static int idtlen;
    static int hexlen;
    int nctable = (sizeof(ctable))/(sizeof(ctable[0]));
    int newlen;

    idtlen = strlen("DIGIT_n_"); /* initial digit template */
    hexlen = strlen("_XHH"); /* template for hex of non-ASCII bytes */
    if(init == 0) {
	int i;
	char *rp;

	for(i = 0; i < 128; i++) {
	    rp = emalloc(2);
	    rp[0] = i;
	    rp[1] = '\0';
	    repls[i] = rp;
	}
	for(i=0; i < nctable; i++) {
	    size_t j = ctable[i].c;
	    free(repls[j]);
	    repls[j] = ctable[i].s;
	}
	for(i = 128; i < 256; i++) {
	    rp = emalloc(hexlen+1);
	    snprintf(rp, hexlen+1, "_X%2.2X", i); /* need to include null*/
	    rp[hexlen] = '\0';
	    repls[i] = rp;
	}
	for(i = 0; i < 256; i++) {
	    lens[i] = strlen(repls[i]);
	}
	init = 1;		/* only do this initialization once */
    }

    count = 0;
    cp = name;
    while(*cp != '\0') {	/* get number of extra bytes for newname */
	size_t j;
        if(((signed char)*cp) < 0) {		/* handle signed or unsigned chars */
	    j = *cp + 256;
	} else {
	    j = *cp;
	}
 	count += lens[j] - 1;
	cp++;
    }

    cp = name;
    if('0' <= *cp && *cp <= '9') { /* names that begin with a digit */
	count += idtlen - 1;
    }
    newlen = strlen(name) + count + 1; /* bytes left to be filled */
    newname = (char *)poolalloc(newlen);
    sp = newname;
    if('0' <= *cp && *cp <= '9') { /* handle initial digit, if any */
	snprintf(sp, newlen, "DIGIT_%c_", *cp);
	sp += idtlen;
	newlen -= idtlen;
	cp++;
    }
    *sp = '\0';
    while(*cp != '\0') { /* copy name to newname, replacing special chars */
	size_t j, len;
	/* cp is current position in name, sp is current position in newname */
        if(((signed char)*cp) < 0) {/* j is table index for character *cp */
	    j = *cp + 256;
	} else {
	    j = *cp;
	}
	len = localstrlcat(sp, repls[j], newlen);
	assert(len < newlen);
	sp += lens[j];
	newlen -= lens[j];
	cp++;
    }
    return newname;
}

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
static size_t
localstrlcat(char *dst,const char *src,size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0') d++;
    dlen = d - dst;
    n = siz - dlen;
    if (n == 0) return(dlen + strlen(s));
    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';
    return(dlen + (s - src)); /* count does not include NUL */
}
