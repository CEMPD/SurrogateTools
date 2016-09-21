/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NC_UNLIMITED_K = 258,
     CHAR_K = 259,
     BYTE_K = 260,
     SHORT_K = 261,
     INT_K = 262,
     FLOAT_K = 263,
     DOUBLE_K = 264,
     UBYTE_K = 265,
     USHORT_K = 266,
     UINT_K = 267,
     INT64_K = 268,
     UINT64_K = 269,
     IDENT = 270,
     TERMSTRING = 271,
     CHAR_CONST = 272,
     BYTE_CONST = 273,
     SHORT_CONST = 274,
     INT_CONST = 275,
     INT64_CONST = 276,
     UBYTE_CONST = 277,
     USHORT_CONST = 278,
     UINT_CONST = 279,
     UINT64_CONST = 280,
     FLOAT_CONST = 281,
     DOUBLE_CONST = 282,
     DIMENSIONS = 283,
     VARIABLES = 284,
     NETCDF = 285,
     DATA = 286,
     TYPES = 287,
     COMPOUND = 288,
     ENUM = 289,
     OPAQUE = 290,
     OPAQUESTRING = 291,
     GROUP = 292,
     PATH = 293,
     FILLMARKER = 294,
     _FILLVALUE = 295,
     _FORMAT = 296,
     _STORAGE = 297,
     _CHUNKSIZES = 298,
     _DEFLATELEVEL = 299,
     _SHUFFLE = 300,
     _ENDIANNESS = 301,
     _NOFILL = 302,
     _FLETCHER32 = 303
   };
#endif
/* Tokens.  */
#define NC_UNLIMITED_K 258
#define CHAR_K 259
#define BYTE_K 260
#define SHORT_K 261
#define INT_K 262
#define FLOAT_K 263
#define DOUBLE_K 264
#define UBYTE_K 265
#define USHORT_K 266
#define UINT_K 267
#define INT64_K 268
#define UINT64_K 269
#define IDENT 270
#define TERMSTRING 271
#define CHAR_CONST 272
#define BYTE_CONST 273
#define SHORT_CONST 274
#define INT_CONST 275
#define INT64_CONST 276
#define UBYTE_CONST 277
#define USHORT_CONST 278
#define UINT_CONST 279
#define UINT64_CONST 280
#define FLOAT_CONST 281
#define DOUBLE_CONST 282
#define DIMENSIONS 283
#define VARIABLES 284
#define NETCDF 285
#define DATA 286
#define TYPES 287
#define COMPOUND 288
#define ENUM 289
#define OPAQUE 290
#define OPAQUESTRING 291
#define GROUP 292
#define PATH 293
#define FILLMARKER 294
#define _FILLVALUE 295
#define _FORMAT 296
#define _STORAGE 297
#define _CHUNKSIZES 298
#define _DEFLATELEVEL 299
#define _SHUFFLE 300
#define _ENDIANNESS 301
#define _NOFILL 302
#define _FLETCHER32 303




/* Copy the first part of user declarations.  */
#line 16 "ncgen.y"

/*
static char SccsId[] = "$Id: ncgen.y,v 1.12 2009/03/11 18:26:18 dmh Exp $";
*/
#include        "includes.h"
#include        "offsets.h"

/* parser controls */
#define YY_NO_INPUT 1

/* True if string a equals string b*/
#define STREQ(a, b)     (*(a) == *(b) && strcmp((a), (b)) == 0)
#define VLENSIZE  (sizeof(nc_vlen_t))
#define MAXFLOATDIM 4294967295.0
#define PRIMNO (NC_STRING - NC_NAT + 1)

/* mnemonic */
typedef enum Attrkind {ATTRVAR, ATTRGLOBAL, DONTKNOW} Attrkind;

typedef nc_vlen_t vlen_t;

/* We retain the old representation of the symbol list
   as a linked list.
*/
Symbol* symlist;

/* Track rootgroup separately*/
Symbol* rootgroup;

/* Track the group sequence */
static List* groupstack;

/* Provide a separate sequence for accumulating values
   during the parse.
*/
static List* stack;

/* track homogeneity of types for data lists*/
static nc_type consttype;

/* Misc. */
static int stackbase;
static int stacklen;
static int count;
static int opaqueid; /* counter for opaque constants*/
static int arrayuid; /* counter for pseudo-array types*/

Symbol* primsymbols[PRIMNO];

char* primtypenames[PRIMNO] = {
"nat",
"byte", "char", "short",
"int", "float", "double",
"ubyte", "ushort", "uint",
"int64", "uint64",
"string"
};

/*Defined in ncgen.l*/
extern int lineno;              /* line number for error messages */
extern char* lextext;           /* name or string with escapes removed */

extern double double_val;       /* last double value read */
extern float float_val;         /* last float value read */
extern long long int64_val;         /* last int64 value read */
extern int int32_val;             /* last int32 value read */
extern short int16_val;         /* last short value read */
extern unsigned long long uint64_val;         /* last int64 value read */
extern unsigned int uint32_val;             /* last int32 value read */
extern unsigned short uint16_val;         /* last short value read */
extern char char_val;           /* last char value read */
extern signed char byte_val;    /* last byte value read */
extern unsigned char ubyte_val;    /* last byte value read */

/* Track definitions of dims, types, attributes, and vars*/
List* grpdefs;
List* dimdefs;
List* attdefs; /* variable-specific attributes*/
List* gattdefs; /* global attributes only*/
List* xattdefs; /* unknown attributes*/
List* typdefs;
List* vardefs;
List* condefs; /* non-dimension constants used in type defs*/
List* tmp;

/* Forward */
static Constant makedataconst(nc_type);
static Constant makeenumconst(Symbol*);
static void addtogroup(Symbol*);
static Symbol* getunlimiteddim(void);
static void setunlimiteddim(Symbol* udim);
static Symbol* currentgroup(void);
static Symbol* createrootgroup(void);
static Symbol* creategroup(Symbol*);
static int dupobjectcheck(nc_class,Symbol*);
static void setpathcurrent(Symbol* sym);
static char* primtypename(nc_type nctype);
static Symbol* makeattribute(Symbol*,Symbol*,Symbol*,Datalist*,Attrkind);
static Symbol* makeprimitivetype(nc_type i);
static Symbol* makespecial(int tag, Symbol* vsym, Symbol* tsym, void* data);
static int containsfills(Datalist* list);

int yylex(void);

#ifndef NO_STDARG
static void yyerror(const char *fmt, ...);
#else
static void yyerror(fmt,va_alist) const char* fmt; va_dcl;
#endif

/* Extern */
extern int lex_init(void);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 133 "ncgen.y"
{
Symbol* sym;
unsigned long  size; /* allow for zero size to indicate e.g. UNLIMITED*/
long           mark; /* track indices into the sequence*/
int            nctype; /* for tracking attribute list type*/
Datalist*      datalist;
Constant       constant;
}
/* Line 187 of yacc.c.  */
#line 316 "ncgen.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 329 "ncgen.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   331

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  130
/* YYNRULES -- Number of states.  */
#define YYNSTATES  228

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      54,    55,    56,     2,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    51,
       2,    53,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,     2,    50,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,     9,    14,    20,    21,    24,    25,
      26,    35,    36,    38,    41,    43,    46,    48,    50,    53,
      56,    59,    62,    65,    72,    74,    78,    82,    88,    94,
     100,   103,   107,   110,   112,   114,   116,   118,   120,   122,
     124,   126,   128,   130,   132,   133,   135,   138,   141,   145,
     147,   149,   151,   155,   159,   163,   167,   169,   170,   172,
     175,   178,   182,   184,   186,   189,   191,   195,   198,   199,
     203,   205,   209,   211,   213,   217,   220,   221,   225,   227,
     231,   233,   235,   237,   239,   241,   242,   246,   251,   258,
     264,   270,   277,   283,   289,   295,   301,   307,   313,   319,
     324,   326,   328,   329,   331,   334,   337,   341,   345,   347,
     351,   353,   357,   359,   361,   363,   365,   367,   369,   371,
     373,   375,   377,   379,   381,   383,   385,   387,   389,   393,
     395
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      59,     0,    -1,    30,    60,    61,    -1,    15,    -1,    49,
      62,    63,    50,    -1,   104,    67,    81,    87,   107,    -1,
      -1,    63,    64,    -1,    -1,    -1,    37,    15,    49,    65,
      62,    63,    66,    50,    -1,    -1,    32,    -1,    32,    68,
      -1,    70,    -1,    68,    70,    -1,    15,    -1,    71,    -1,
     105,    51,    -1,    72,    51,    -1,    77,    51,    -1,    76,
      51,    -1,    75,    51,    -1,    80,    34,    69,    49,    73,
      50,    -1,    74,    -1,    73,    52,    74,    -1,    15,    53,
     112,    -1,    35,    54,    20,    55,    69,    -1,   102,    54,
      56,    55,    69,    -1,    33,    69,    49,    78,    50,    -1,
      79,    51,    -1,    78,    79,    51,    -1,   102,    96,    -1,
       4,    -1,     5,    -1,     6,    -1,     7,    -1,     8,    -1,
       9,    -1,    10,    -1,    11,    -1,    12,    -1,    13,    -1,
      14,    -1,    -1,    28,    -1,    28,    82,    -1,    83,    51,
      -1,    82,    83,    51,    -1,    84,    -1,   105,    -1,    85,
      -1,    84,    52,    85,    -1,    86,    53,    20,    -1,    86,
      53,    27,    -1,    86,    53,     3,    -1,    15,    -1,    -1,
      29,    -1,    29,    88,    -1,    89,    51,    -1,    88,    89,
      51,    -1,    90,    -1,   105,    -1,   102,    91,    -1,    92,
      -1,    91,    52,    92,    -1,    15,    93,    -1,    -1,    54,
      94,    55,    -1,    95,    -1,    94,    52,    95,    -1,   106,
      -1,    97,    -1,    96,    52,    97,    -1,    15,    98,    -1,
      -1,    54,    99,    55,    -1,   100,    -1,    99,    52,   100,
      -1,    20,    -1,   103,    -1,   103,    -1,   106,    -1,    80,
      -1,    -1,   105,    51,   104,    -1,    57,    15,    53,   110,
      -1,   102,   103,    57,    15,    53,   110,    -1,   103,    57,
      15,    53,   110,    -1,   103,    57,    40,    53,   110,    -1,
     102,   103,    57,    40,    53,   110,    -1,   103,    57,    42,
      53,   115,    -1,   103,    57,    43,    53,   113,    -1,   103,
      57,    48,    53,   115,    -1,   103,    57,    44,    53,   114,
      -1,   103,    57,    45,    53,   115,    -1,   103,    57,    46,
      53,   115,    -1,   103,    57,    47,    53,   115,    -1,    57,
      41,    53,   115,    -1,    15,    -1,    38,    -1,    -1,    31,
      -1,    31,   108,    -1,   109,    51,    -1,   108,   109,    51,
      -1,   101,    53,   110,    -1,   111,    -1,   110,    52,   111,
      -1,   112,    -1,    49,   110,    50,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,    16,
      -1,    36,    -1,   106,    -1,    39,    -1,   114,    -1,   113,
      52,   114,    -1,    20,    -1,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   207,   207,   213,   215,   222,   229,   229,   232,   240,
     231,   243,   244,   245,   248,   248,   251,   261,   261,   263,
     263,   263,   263,   265,   295,   297,   314,   323,   334,   347,
     379,   380,   383,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   410,   411,   412,   415,   416,   419,
     419,   421,   422,   425,   430,   440,   452,   464,   465,   466,
     469,   470,   473,   473,   475,   497,   501,   505,   532,   533,
     536,   537,   541,   555,   559,   564,   593,   594,   598,   599,
     604,   620,   631,   642,   661,   668,   668,   671,   673,   682,
     693,   695,   697,   699,   701,   703,   705,   707,   709,   711,
     716,   722,   731,   732,   733,   736,   737,   740,   745,   746,
     750,   751,   755,   756,   757,   758,   759,   760,   761,   762,
     763,   764,   765,   766,   767,   768,   769,   773,   774,   778,
     782
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NC_UNLIMITED_K", "CHAR_K", "BYTE_K",
  "SHORT_K", "INT_K", "FLOAT_K", "DOUBLE_K", "UBYTE_K", "USHORT_K",
  "UINT_K", "INT64_K", "UINT64_K", "IDENT", "TERMSTRING", "CHAR_CONST",
  "BYTE_CONST", "SHORT_CONST", "INT_CONST", "INT64_CONST", "UBYTE_CONST",
  "USHORT_CONST", "UINT_CONST", "UINT64_CONST", "FLOAT_CONST",
  "DOUBLE_CONST", "DIMENSIONS", "VARIABLES", "NETCDF", "DATA", "TYPES",
  "COMPOUND", "ENUM", "OPAQUE", "OPAQUESTRING", "GROUP", "PATH",
  "FILLMARKER", "_FILLVALUE", "_FORMAT", "_STORAGE", "_CHUNKSIZES",
  "_DEFLATELEVEL", "_SHUFFLE", "_ENDIANNESS", "_NOFILL", "_FLETCHER32",
  "'{'", "'}'", "';'", "','", "'='", "'('", "')'", "'*'", "':'", "$accept",
  "ncdesc", "datasetname", "rootgroup", "groupbody", "subgrouplist",
  "namedgroup", "@1", "@2", "typesection", "typedecls", "typename",
  "type_attr_decl", "typedecl", "enumdecl", "enumidlist", "enumid",
  "opaquedecl", "vlendecl", "compounddecl", "fields", "field", "primtype",
  "dimsection", "dimdecls", "dim_attr_decl", "dimdeclist", "dimdecl",
  "dimd", "vasection", "vadecls", "vadecl", "vardecl", "varlist",
  "varspec", "dimspec", "dimlist", "dimref", "fieldlist", "fieldspec",
  "fielddimspec", "fielddimlist", "fielddim", "varref", "typeref",
  "type_var_ref", "attrdecllist", "attrdecl", "path", "datasection",
  "datadecls", "datadecl", "datalist", "dataset", "dataconst",
  "integerlist", "integerconst", "stringconst", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   123,
     125,    59,    44,    61,    40,    41,    42,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    58,    59,    60,    61,    62,    63,    63,    65,    66,
      64,    67,    67,    67,    68,    68,    69,    70,    70,    71,
      71,    71,    71,    72,    73,    73,    74,    75,    76,    77,
      78,    78,    79,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    81,    81,    81,    82,    82,    83,
      83,    84,    84,    85,    85,    85,    86,    87,    87,    87,
      88,    88,    89,    89,    90,    91,    91,    92,    93,    93,
      94,    94,    95,    96,    96,    97,    98,    98,    99,    99,
     100,   101,   102,   103,   103,   104,   104,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     106,   106,   107,   107,   107,   108,   108,   109,   110,   110,
     111,   111,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   113,   113,   114,
     115
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     1,     4,     5,     0,     2,     0,     0,
       8,     0,     1,     2,     1,     2,     1,     1,     2,     2,
       2,     2,     2,     6,     1,     3,     3,     5,     5,     5,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     2,     2,     3,     1,
       1,     1,     3,     3,     3,     3,     1,     0,     1,     2,
       2,     3,     1,     1,     2,     1,     3,     2,     0,     3,
       1,     3,     1,     1,     3,     2,     0,     3,     1,     3,
       1,     1,     1,     1,     1,     0,     3,     4,     6,     5,
       5,     6,     5,     5,     5,     5,     5,     5,     5,     4,
       1,     1,     0,     1,     2,     2,     3,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     3,     0,     1,    85,     2,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,   100,
     101,     0,     6,    84,     0,    82,    11,     0,    83,     0,
       0,     0,     0,     0,    12,    44,    85,     0,     0,     0,
       4,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    13,    14,    17,     0,     0,     0,
       0,    84,     0,     0,    45,    57,    86,   123,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   124,
     126,     0,   125,    87,   108,   110,   130,    99,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      16,     0,     0,    15,    19,    22,    21,    20,     0,     0,
      18,   100,    46,     0,    49,    51,     0,    50,    58,   102,
       0,     0,     8,     0,     0,    89,    90,    92,   129,    93,
     127,    95,    96,    97,    98,    94,     0,     0,     0,     0,
       0,    47,     0,     0,    59,     0,    62,     0,    63,   103,
       5,   111,   109,    85,    88,    91,     0,     0,     0,     0,
      82,     0,     0,     0,    48,    56,    52,    55,    53,    54,
       0,    60,    68,    64,    65,     0,    81,   104,     0,     6,
     128,    29,     0,    30,    76,    32,    73,    27,     0,     0,
      24,    28,    61,     0,    67,     0,     0,     0,   105,     9,
      31,     0,    75,     0,     0,    23,     0,     0,    70,    72,
      68,    66,   107,   106,     0,    80,     0,    78,    74,    26,
      25,     0,    69,    10,     0,    77,    71,    79
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     4,     7,    22,    31,    41,   153,   214,    35,
      54,   101,    55,    56,    57,   189,   190,    58,    59,    60,
     157,   158,    23,    65,   112,   113,   114,   115,   116,   119,
     144,   145,   146,   173,   174,   194,   207,   208,   185,   186,
     202,   216,   217,   175,    24,    25,    26,    27,    28,   150,
     177,   178,    83,    84,    85,   129,   130,    87
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -103
static const yytype_int16 yypact[] =
{
     -19,     2,    18,  -103,   -17,  -103,   131,  -103,  -103,  -103,
    -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,
    -103,    -8,  -103,  -103,   256,    -5,    10,     0,  -103,   -26,
     -10,   -22,    -4,   118,    60,    28,   131,   210,    41,    43,
    -103,  -103,    -6,     7,    23,    24,    25,    26,    27,    29,
      32,    37,    47,    38,    60,  -103,  -103,    30,    40,    45,
      46,    65,   186,    49,   166,    72,  -103,  -103,  -103,  -103,
    -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,
    -103,   210,  -103,    42,  -103,  -103,  -103,  -103,    53,    50,
      51,   210,   210,    41,    85,    85,    41,    41,    41,    41,
    -103,    58,    88,  -103,  -103,  -103,  -103,  -103,    47,    54,
    -103,    56,   166,    61,    59,  -103,    63,  -103,   131,    89,
      -3,   210,  -103,   210,   210,    42,    42,  -103,  -103,    67,
    -103,  -103,  -103,  -103,  -103,  -103,   256,    66,    73,    68,
      74,  -103,   113,     9,   131,    78,  -103,   268,  -103,   256,
    -103,  -103,  -103,   131,    42,    42,    85,   201,    79,   116,
    -103,    47,   117,    47,  -103,  -103,  -103,  -103,  -103,  -103,
      96,  -103,   -31,    97,  -103,    95,  -103,   256,    99,  -103,
    -103,  -103,   100,  -103,    98,   101,  -103,  -103,   102,    -2,
    -103,  -103,  -103,    -7,  -103,   142,   210,   132,  -103,   145,
    -103,   148,  -103,   116,   292,  -103,   117,   -15,  -103,  -103,
     133,  -103,    42,  -103,   135,  -103,   -14,  -103,  -103,  -103,
    -103,    -7,  -103,  -103,   148,  -103,  -103,  -103
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -103,  -103,  -103,  -103,    33,    39,  -103,  -103,  -103,  -103,
    -103,  -102,   149,  -103,  -103,  -103,    11,  -103,  -103,  -103,
    -103,    62,   -24,  -103,  -103,    77,  -103,    80,  -103,  -103,
    -103,    76,  -103,  -103,    48,  -103,  -103,    17,  -103,    -1,
    -103,  -103,    20,  -103,   -30,   -23,   185,   -29,   -37,  -103,
    -103,    64,   -78,   121,    52,  -103,   -93,   -77
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -101
static const yytype_int16 yytable[] =
{
      82,    32,   131,   120,    62,    63,   138,    29,    19,    89,
      61,     1,   167,   125,   126,    39,   127,     3,     5,   132,
     133,   134,   135,   193,    62,    63,  -100,    37,    40,   168,
      61,    20,     6,    30,    90,   117,   169,   221,   224,    32,
     222,   225,    34,    38,    82,   154,   155,   151,   205,   121,
     206,    36,    33,    42,    82,    82,    64,    86,    88,   187,
      91,   191,   100,   180,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    92,    93,    94,    95,
      96,   104,    97,   117,    82,    98,    82,    82,   147,   148,
      99,   105,   102,    52,   121,    53,   106,   107,    20,   108,
     110,   118,   122,   123,   124,   128,   159,   136,   137,   -56,
     139,   142,   141,   160,   147,   148,   143,    21,   212,   156,
     149,   161,   162,   163,    32,   164,   176,   159,   165,   171,
     183,   184,   188,    43,   160,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,   192,   196,   195,
     198,   200,   201,   203,   176,   204,   209,   210,    44,    82,
      45,    46,    47,    48,    49,    50,    51,    82,   215,    20,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,   111,    39,   213,   209,   223,   179,   193,    21,   140,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,   218,   103,    20,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,   220,   199,   182,
     170,    66,   166,    21,    20,    19,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,   226,    20,
     109,   197,   152,   211,   227,     0,    79,     0,    20,    80,
       0,   181,     0,     0,     0,     0,   219,     0,     0,    81,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,   172,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    19,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
       0,     0,     0,     0,     0,     0,     0,     0,    79,     0,
      20,    80
};

static const yytype_int16 yycheck[] =
{
      37,    24,    95,    81,    34,    34,   108,    15,    15,    15,
      34,    30,     3,    91,    92,    37,    93,    15,     0,    96,
      97,    98,    99,    54,    54,    54,    57,    53,    50,    20,
      54,    38,    49,    41,    40,    64,    27,    52,    52,    62,
      55,    55,    32,    53,    81,   123,   124,    50,    50,    52,
      52,    51,    57,    57,    91,    92,    28,    16,    15,   161,
      53,   163,    15,   156,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    53,    53,    53,    53,
      53,    51,    53,   112,   121,    53,   123,   124,   118,   118,
      53,    51,    54,    33,    52,    35,    51,    51,    38,    34,
      51,    29,    49,    53,    53,    20,   136,    49,    20,    53,
      56,    52,    51,   136,   144,   144,    53,    57,   196,    52,
      31,    55,    49,    55,   147,    51,   149,   157,    15,    51,
      51,    15,    15,    15,   157,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    51,    53,    52,
      51,    51,    54,    52,   177,    53,   193,    15,    40,   196,
      42,    43,    44,    45,    46,    47,    48,   204,    20,    38,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    37,    51,   221,    50,   153,    54,    57,   112,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,   203,    54,    38,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,   206,   179,   157,
     144,    36,   142,    57,    38,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,   221,    38,
      54,   177,   121,   195,   224,    -1,    36,    -1,    38,    39,
      -1,    50,    -1,    -1,    -1,    -1,   204,    -1,    -1,    49,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,
      38,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    30,    59,    15,    60,     0,    49,    61,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      38,    57,    62,    80,   102,   103,   104,   105,   106,    15,
      41,    63,   103,    57,    32,    67,    51,    53,    53,    37,
      50,    64,    57,    15,    40,    42,    43,    44,    45,    46,
      47,    48,    33,    35,    68,    70,    71,    72,    75,    76,
      77,    80,   102,   105,    28,    81,   104,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    36,
      39,    49,   106,   110,   111,   112,    16,   115,    15,    15,
      40,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      15,    69,    54,    70,    51,    51,    51,    51,    34,    54,
      51,    15,    82,    83,    84,    85,    86,   105,    29,    87,
     110,    52,    49,    53,    53,   110,   110,   115,    20,   113,
     114,   114,   115,   115,   115,   115,    49,    20,    69,    56,
      83,    51,    52,    53,    88,    89,    90,   102,   105,    31,
     107,    50,   111,    65,   110,   110,    52,    78,    79,   102,
     103,    55,    49,    55,    51,    15,    85,     3,    20,    27,
      89,    51,    15,    91,    92,   101,   103,   108,   109,    62,
     114,    50,    79,    51,    15,    96,    97,    69,    15,    73,
      74,    69,    51,    54,    93,    52,    53,   109,    51,    63,
      51,    54,    98,    52,    53,    50,    52,    94,    95,   106,
      15,    92,   110,    51,    66,    20,    99,   100,    97,   112,
      74,    52,    55,    50,    52,    55,    95,   100
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 210 "ncgen.y"
    {if (derror_count > 0) exit(6);;}
    break;

  case 3:
#line 213 "ncgen.y"
    {datasetname = strdup((yyvsp[(1) - (1)].sym)->name);;}
    break;

  case 8:
#line 232 "ncgen.y"
    {
                if(usingclassic) {verror("Group specification");}
		if(creategroup((yyvsp[(2) - (3)].sym)) == NULL) 
                    yyerror("duplicate group declaration within parent group for %s",
                                (yyvsp[(2) - (3)].sym)->name);
            ;}
    break;

  case 9:
#line 240 "ncgen.y"
    {listpop(groupstack);;}
    break;

  case 12:
#line 244 "ncgen.y"
    {;}
    break;

  case 13:
#line 245 "ncgen.y"
    {if(usingclassic)verror("Type specification");;}
    break;

  case 16:
#line 252 "ncgen.y"
    { /* Use when defining a type */
              (yyvsp[(1) - (1)].sym)->objectclass = NC_TYPE;
              if(dupobjectcheck(NC_TYPE,(yyvsp[(1) - (1)].sym)))
                    yyerror("duplicate type declaration for %s",
                            (yyvsp[(1) - (1)].sym)->name);
              listpush(typdefs,(elem_t)(yyvsp[(1) - (1)].sym));
	    ;}
    break;

  case 17:
#line 261 "ncgen.y"
    {;}
    break;

  case 18:
#line 261 "ncgen.y"
    {;}
    break;

  case 23:
#line 267 "ncgen.y"
    {
		int i;
                addtogroup((yyvsp[(3) - (6)].sym)); /* sets prefix*/
                (yyvsp[(3) - (6)].sym)->objectclass=NC_TYPE;
                (yyvsp[(3) - (6)].sym)->subclass=NC_ENUM;
                (yyvsp[(3) - (6)].sym)->typ.basetype=(yyvsp[(1) - (6)].sym);
                (yyvsp[(3) - (6)].sym)->typ.size = (yyvsp[(1) - (6)].sym)->typ.size;
                (yyvsp[(3) - (6)].sym)->typ.alignment = (yyvsp[(1) - (6)].sym)->typ.alignment;
                stackbase=(yyvsp[(5) - (6)].mark);
                stacklen=listlength(stack);
                (yyvsp[(3) - (6)].sym)->subnodes = listnew();
                /* Variety of field fixups*/
		/* 1. add in the enum values*/
		/* 2. make this type be their container*/
		/* 3. make constant names visible in the group*/
		/* 4. set field basetype to be same as enum basetype*/
                for(i=stackbase;i<stacklen;i++) {
                   Symbol* eid = (Symbol*)listget(stack,i);
		   assert(eid->subclass == NC_ECONST);
		   addtogroup(eid);
                   listpush((yyvsp[(3) - (6)].sym)->subnodes,(elem_t)eid);
                   eid->container = (yyvsp[(3) - (6)].sym);
		   eid->typ.basetype = (yyvsp[(3) - (6)].sym)->typ.basetype;
                }               
                listsetlength(stack,stackbase);/* remove stack nodes*/
              ;}
    break;

  case 24:
#line 296 "ncgen.y"
    {(yyval.mark)=listlength(stack); listpush(stack,(elem_t)(yyvsp[(1) - (1)].sym));;}
    break;

  case 25:
#line 298 "ncgen.y"
    {
		    int i;
		    (yyval.mark)=(yyvsp[(1) - (3)].mark);
		    /* check for duplicates*/
		    stackbase=(yyvsp[(1) - (3)].mark);
		    stacklen=listlength(stack);
		    for(i=stackbase;i<stacklen;i++) {
		      Symbol* elem = (Symbol*)listget(stack,i);
		      if(strcmp((yyvsp[(3) - (3)].sym)->name,elem->name)==0)
  	                yyerror("duplicate enum declaration for %s",
        	                 elem->name);
		    }    	    
		    listpush(stack,(elem_t)(yyvsp[(3) - (3)].sym));
		;}
    break;

  case 26:
#line 315 "ncgen.y"
    {
            (yyvsp[(1) - (3)].sym)->objectclass=NC_TYPE;
            (yyvsp[(1) - (3)].sym)->subclass=NC_ECONST;
            (yyvsp[(1) - (3)].sym)->typ.econst=(yyvsp[(3) - (3)].constant);
	    (yyval.sym)=(yyvsp[(1) - (3)].sym);
        ;}
    break;

  case 27:
#line 324 "ncgen.y"
    {
                    addtogroup((yyvsp[(5) - (5)].sym)); /*sets prefix*/
                    (yyvsp[(5) - (5)].sym)->objectclass=NC_TYPE;
                    (yyvsp[(5) - (5)].sym)->subclass=NC_OPAQUE;
                    (yyvsp[(5) - (5)].sym)->typ.typecode=NC_OPAQUE;
                    (yyvsp[(5) - (5)].sym)->typ.size=int32_val;
                    (yyvsp[(5) - (5)].sym)->typ.alignment=nctypealignment(NC_OPAQUE);
                ;}
    break;

  case 28:
#line 335 "ncgen.y"
    {
                    Symbol* basetype = (yyvsp[(1) - (5)].sym);
                    addtogroup((yyvsp[(5) - (5)].sym)); /*sets prefix*/
                    (yyvsp[(5) - (5)].sym)->objectclass=NC_TYPE;
                    (yyvsp[(5) - (5)].sym)->subclass=NC_VLEN;
                    (yyvsp[(5) - (5)].sym)->typ.basetype=basetype;
                    (yyvsp[(5) - (5)].sym)->typ.typecode=NC_VLEN;
                    (yyvsp[(5) - (5)].sym)->typ.size=VLENSIZE;
                    (yyvsp[(5) - (5)].sym)->typ.alignment=nctypealignment(NC_VLEN);
                ;}
    break;

  case 29:
#line 348 "ncgen.y"
    {
	    int i,j;
            addtogroup((yyvsp[(2) - (5)].sym));
	    /* check for duplicate field names*/
	    stackbase=(yyvsp[(4) - (5)].mark);
	    stacklen=listlength(stack);
	    for(i=stackbase;i<stacklen;i++) {
	      Symbol* elem1 = (Symbol*)listget(stack,i);
	      for(j=i+1;j<stacklen;j++) {
	          Symbol* elem2 = (Symbol*)listget(stack,j);
	          if(strcmp(elem1->name,elem2->name)==0) {
	            yyerror("duplicate field declaration for %s",elem1->name);
		  }
	      }
	    }
	    (yyvsp[(2) - (5)].sym)->objectclass=NC_TYPE;
            (yyvsp[(2) - (5)].sym)->subclass=NC_COMPOUND;
            (yyvsp[(2) - (5)].sym)->typ.basetype=NULL;
            (yyvsp[(2) - (5)].sym)->typ.typecode=NC_COMPOUND;
	    (yyvsp[(2) - (5)].sym)->subnodes = listnew();
	    /* Add in the fields*/
	    for(i=stackbase;i<stacklen;i++) {
	        Symbol* fsym = (Symbol*)listget(stack,i);
		fsym->container = (yyvsp[(2) - (5)].sym);
 	        listpush((yyvsp[(2) - (5)].sym)->subnodes,(elem_t)fsym);
	    }    	    
	    listsetlength(stack,stackbase);/* remove stack nodes*/
          ;}
    break;

  case 30:
#line 379 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (2)].mark);;}
    break;

  case 31:
#line 380 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (3)].mark);;}
    break;

  case 32:
#line 384 "ncgen.y"
    {
	    int i;
	    (yyval.mark)=(yyvsp[(2) - (2)].mark);
	    stackbase=(yyvsp[(2) - (2)].mark);
	    stacklen=listlength(stack);
	    /* process each field in the fieldlist*/
            for(i=stackbase;i<stacklen;i++) {
                Symbol* f = (Symbol*)listget(stack,i);
		f->typ.basetype = (yyvsp[(1) - (2)].sym);
            }
        ;}
    break;

  case 33:
#line 397 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_CHAR]; ;}
    break;

  case 34:
#line 398 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_BYTE]; ;}
    break;

  case 35:
#line 399 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_SHORT]; ;}
    break;

  case 36:
#line 400 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_INT]; ;}
    break;

  case 37:
#line 401 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_FLOAT]; ;}
    break;

  case 38:
#line 402 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_DOUBLE]; ;}
    break;

  case 39:
#line 403 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_UBYTE]; ;}
    break;

  case 40:
#line 404 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_USHORT]; ;}
    break;

  case 41:
#line 405 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_UINT]; ;}
    break;

  case 42:
#line 406 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_INT64]; ;}
    break;

  case 43:
#line 407 "ncgen.y"
    { (yyval.sym) = primsymbols[NC_UINT64]; ;}
    break;

  case 45:
#line 411 "ncgen.y"
    {;}
    break;

  case 46:
#line 412 "ncgen.y"
    {;}
    break;

  case 49:
#line 419 "ncgen.y"
    {;}
    break;

  case 50:
#line 419 "ncgen.y"
    {;}
    break;

  case 53:
#line 426 "ncgen.y"
    { if (int32_val <= 0)
                         yyerror("dimension length must be positive");
                     (yyvsp[(1) - (3)].sym)->dim.size = (size_t)int32_val;
                   ;}
    break;

  case 54:
#line 431 "ncgen.y"
    { /* for rare case where 2^31 < dimsize < 2^32 */
                       if (double_val <= 0)
                         yyerror("dimension length must be positive");
                       if (double_val > MAXFLOATDIM)
                         yyerror("dimension too large");
                       if (double_val - (size_t) double_val > 0)
                         yyerror("dimension length must be an integer");
                       (yyvsp[(1) - (3)].sym)->dim.size = (size_t)double_val;
                   ;}
    break;

  case 55:
#line 441 "ncgen.y"
    {
                       if(usingclassic) {
	  	         /* check for multiple UNLIMITED decls*/
                         if(getunlimiteddim() != NULL)
			    verror("Type specification");
			 setunlimiteddim((yyvsp[(1) - (3)].sym));
		       }
		       (yyvsp[(1) - (3)].sym)->dim.size = NC_UNLIMITED;
		   ;}
    break;

  case 56:
#line 453 "ncgen.y"
    { 
                     (yyvsp[(1) - (1)].sym)->objectclass=NC_DIM;
                     if(dupobjectcheck(NC_DIM,(yyvsp[(1) - (1)].sym)))
                        yyerror( "Duplicate dimension declaration for %s",
                                (yyvsp[(1) - (1)].sym)->name);
		     addtogroup((yyvsp[(1) - (1)].sym));
		     (yyval.sym)=(yyvsp[(1) - (1)].sym);
		     listpush(dimdefs,(elem_t)(yyvsp[(1) - (1)].sym));
                   ;}
    break;

  case 58:
#line 465 "ncgen.y"
    {;}
    break;

  case 59:
#line 466 "ncgen.y"
    {;}
    break;

  case 62:
#line 473 "ncgen.y"
    {;}
    break;

  case 63:
#line 473 "ncgen.y"
    {;}
    break;

  case 64:
#line 476 "ncgen.y"
    {
		    int i;
		    stackbase=(yyvsp[(2) - (2)].mark);
		    stacklen=listlength(stack);
		    /* process each variable in the varlist*/
	            for(i=stackbase;i<stacklen;i++) {
	                Symbol* sym = (Symbol*)listget(stack,i);
			sym->objectclass = NC_VAR;
		        if(dupobjectcheck(NC_VAR,sym)) {
                            yyerror("Duplicate variable declaration for %s",
                                    sym->name);
			} else {
		  	    sym->typ.basetype = (yyvsp[(1) - (2)].sym);
	                    addtogroup(sym);
		            listpush(vardefs,(elem_t)sym);
			}
		    }
		    listsetlength(stack,stackbase);/* remove stack nodes*/
		;}
    break;

  case 65:
#line 498 "ncgen.y"
    {(yyval.mark)=listlength(stack);
                 listpush(stack,(elem_t)(yyvsp[(1) - (1)].sym));
		;}
    break;

  case 66:
#line 502 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (3)].mark); listpush(stack,(elem_t)(yyvsp[(3) - (3)].sym));;}
    break;

  case 67:
#line 506 "ncgen.y"
    {
		    int i;
		    Dimset dimset;
		    stacklen=listlength(stack);
		    stackbase=(yyvsp[(2) - (2)].mark);
		    count = stacklen - stackbase;
		    if(count >= NC_MAX_VAR_DIMS) {
			yyerror("%s has too many dimensions",(yyvsp[(1) - (2)].sym)->name);
			count = NC_MAX_VAR_DIMS - 1;
			stacklen = stackbase + count;
		    }
  	            dimset.ndims = count;
		    /* extract the actual dimensions*/
		    if(dimset.ndims > 0) {
		        for(i=0;i<count;i++) {
			    Symbol* dsym = (Symbol*)listget(stack,stackbase+i);
			    dimset.dimsyms[i] = dsym;
			}
			(yyvsp[(1) - (2)].sym)->typ.dimset = dimset;
		    }
		    (yyvsp[(1) - (2)].sym)->typ.basetype = NULL; /* not yet known*/
                    (yyvsp[(1) - (2)].sym)->objectclass=NC_VAR;
		    listsetlength(stack,stackbase);/* remove stack nodes*/
		    ;}
    break;

  case 68:
#line 532 "ncgen.y"
    {(yyval.mark)=listlength(stack);;}
    break;

  case 69:
#line 533 "ncgen.y"
    {(yyval.mark)=(yyvsp[(2) - (3)].mark);;}
    break;

  case 70:
#line 536 "ncgen.y"
    {(yyval.mark)=listlength(stack); listpush(stack,(elem_t)(yyvsp[(1) - (1)].sym));;}
    break;

  case 71:
#line 538 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (3)].mark); listpush(stack,(elem_t)(yyvsp[(3) - (3)].sym));;}
    break;

  case 72:
#line 542 "ncgen.y"
    {Symbol* dimsym = (yyvsp[(1) - (1)].sym);
		dimsym->objectclass = NC_DIM;
		/* Find the actual dimension*/
		dimsym = locate(dimsym);
		if(dimsym == NULL) {
		    derror("Undefined or forward referenced dimension: %s",(yyvsp[(1) - (1)].sym)->name);
		    YYABORT;
		}
		(yyval.sym)=dimsym;
	    ;}
    break;

  case 73:
#line 556 "ncgen.y"
    {(yyval.mark)=listlength(stack);
             listpush(stack,(elem_t)(yyvsp[(1) - (1)].sym));
	    ;}
    break;

  case 74:
#line 560 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (3)].mark); listpush(stack,(elem_t)(yyvsp[(3) - (3)].sym));;}
    break;

  case 75:
#line 565 "ncgen.y"
    {
		int i;
		Dimset dimset;
		stackbase=(yyvsp[(2) - (2)].mark);
		stacklen=listlength(stack);
		count = stacklen - stackbase;
		if(count >= NC_MAX_VAR_DIMS) {
		    yyerror("%s has too many dimensions",(yyvsp[(1) - (2)].sym)->name);
		    count = NC_MAX_VAR_DIMS - 1;
		    stacklen = stackbase + count;
		}
  	        dimset.ndims = count;
		if(count > 0) {
		    /* extract the actual dimensions*/
		    for(i=0;i<count;i++) {
		        Symbol* dsym = (Symbol*)listget(stack,stackbase+i);
		        dimset.dimsyms[i] = dsym;
		    }
		    (yyvsp[(1) - (2)].sym)->typ.dimset = dimset;
		}
		(yyvsp[(1) - (2)].sym)->typ.basetype = NULL; /* not yet known*/
                (yyvsp[(1) - (2)].sym)->objectclass=NC_TYPE;
                (yyvsp[(1) - (2)].sym)->subclass=NC_FIELD;
		listsetlength(stack,stackbase);/* remove stack nodes*/
		(yyval.sym) = (yyvsp[(1) - (2)].sym);
	    ;}
    break;

  case 76:
#line 593 "ncgen.y"
    {(yyval.mark)=listlength(stack);;}
    break;

  case 77:
#line 594 "ncgen.y"
    {(yyval.mark)=(yyvsp[(2) - (3)].mark);;}
    break;

  case 78:
#line 598 "ncgen.y"
    {(yyval.mark)=listlength(stack); listpush(stack,(elem_t)(yyvsp[(1) - (1)].sym));;}
    break;

  case 79:
#line 600 "ncgen.y"
    {(yyval.mark)=(yyvsp[(1) - (3)].mark); listpush(stack,(elem_t)(yyvsp[(3) - (3)].sym));;}
    break;

  case 80:
#line 605 "ncgen.y"
    {  /* Anonymous integer dimension.
	         Can only occur in type definitions*/
	     char anon[32];
	     sprintf(anon,"const%d",int32_val);
	     (yyval.sym) = install(anon);
	     (yyval.sym)->objectclass = NC_DIM;
	     (yyval.sym)->dim.isconstant = 1;
	     (yyval.sym)->dim.size = int32_val;
	    ;}
    break;

  case 81:
#line 621 "ncgen.y"
    {Symbol* vsym = (yyvsp[(1) - (1)].sym);
		if(vsym->objectclass != NC_VAR) {
		    derror("Undefined or forward referenced variable: %s",vsym->name);
		    YYABORT;
		}
		(yyval.sym)=vsym;
	    ;}
    break;

  case 82:
#line 632 "ncgen.y"
    {Symbol* tsym = (yyvsp[(1) - (1)].sym);
		if(tsym->objectclass != NC_TYPE) {
		    derror("Undefined or forward referenced type: %s",tsym->name);
		    YYABORT;
		}
		(yyval.sym)=tsym;
	    ;}
    break;

  case 83:
#line 643 "ncgen.y"
    {Symbol* tvsym = (yyvsp[(1) - (1)].sym); Symbol* sym;
		/* disambiguate*/
		tvsym->objectclass = NC_VAR;
		sym = locate(tvsym);
		if(sym == NULL) {
		    tvsym->objectclass = NC_TYPE;
		    sym = locate(tvsym);
		    if(tvsym == NULL) {
		        derror("Undefined or forward referenced name: %s",(yyvsp[(1) - (1)].sym)->name);
		        YYABORT;
		    } else tvsym = sym;
		} else tvsym = sym;
		if(tvsym == NULL) {
		    derror("Undefined name: %s",(yyvsp[(1) - (1)].sym)->name);
		    YYABORT;
		}
		(yyval.sym)=tvsym;
	    ;}
    break;

  case 84:
#line 661 "ncgen.y"
    {(yyval.sym)=(yyvsp[(1) - (1)].sym);;}
    break;

  case 85:
#line 668 "ncgen.y"
    {;}
    break;

  case 86:
#line 668 "ncgen.y"
    {;}
    break;

  case 87:
#line 672 "ncgen.y"
    { (yyval.sym)=makeattribute((yyvsp[(2) - (4)].sym),NULL,NULL,(yyvsp[(4) - (4)].datalist),ATTRGLOBAL);;}
    break;

  case 88:
#line 674 "ncgen.y"
    {Symbol* tsym = (yyvsp[(1) - (6)].sym); Symbol* vsym = (yyvsp[(2) - (6)].sym); Symbol* asym = (yyvsp[(4) - (6)].sym);
		if(vsym->objectclass == NC_VAR) {
		    (yyval.sym)=makeattribute(asym,vsym,tsym,(yyvsp[(6) - (6)].datalist),ATTRVAR);
		} else {
		    derror("Doubly typed attribute: %s",asym->name);
		    YYABORT;
		}
	    ;}
    break;

  case 89:
#line 683 "ncgen.y"
    {Symbol* sym = (yyvsp[(1) - (5)].sym); Symbol* asym = (yyvsp[(3) - (5)].sym);
		if(sym->objectclass == NC_VAR) {
		    (yyval.sym)=makeattribute(asym,sym,NULL,(yyvsp[(5) - (5)].datalist),ATTRVAR);
		} else if(sym->objectclass == NC_TYPE) {
		    (yyval.sym)=makeattribute(asym,NULL,sym,(yyvsp[(5) - (5)].datalist),ATTRGLOBAL);
		} else {
		    derror("Attribute prefix not a variable or type: %s",asym->name);
		    YYABORT;
		}
	    ;}
    break;

  case 90:
#line 694 "ncgen.y"
    {(yyval.sym) = makespecial(_FILLVALUE,(yyvsp[(1) - (5)].sym),NULL,(void*)(yyvsp[(5) - (5)].datalist));;}
    break;

  case 91:
#line 696 "ncgen.y"
    {(yyval.sym) = makespecial(_FILLVALUE,(yyvsp[(2) - (6)].sym),(yyvsp[(1) - (6)].sym),(void*)(yyvsp[(6) - (6)].datalist));;}
    break;

  case 92:
#line 698 "ncgen.y"
    {(yyval.sym) = makespecial(_STORAGE,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 93:
#line 700 "ncgen.y"
    {(yyval.sym) = makespecial(_CHUNKSIZES,(yyvsp[(1) - (5)].sym),NULL,(void*)(yyvsp[(5) - (5)].datalist));;}
    break;

  case 94:
#line 702 "ncgen.y"
    {(yyval.sym) = makespecial(_FLETCHER32,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 95:
#line 704 "ncgen.y"
    {(yyval.sym) = makespecial(_DEFLATELEVEL,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 96:
#line 706 "ncgen.y"
    {(yyval.sym) = makespecial(_SHUFFLE,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 97:
#line 708 "ncgen.y"
    {(yyval.sym) = makespecial(_ENDIANNESS,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 98:
#line 710 "ncgen.y"
    {(yyval.sym) = makespecial(_NOFILL,(yyvsp[(1) - (5)].sym),NULL,(void*)&(yyvsp[(5) - (5)].constant));;}
    break;

  case 99:
#line 712 "ncgen.y"
    {(yyval.sym) = makespecial(_FORMAT,NULL,NULL,(void*)&(yyvsp[(4) - (4)].constant));;}
    break;

  case 100:
#line 717 "ncgen.y"
    {
	        (yyval.sym)=(yyvsp[(1) - (1)].sym);
                (yyvsp[(1) - (1)].sym)->is_ref=1;
                setpathcurrent((yyvsp[(1) - (1)].sym));
	    ;}
    break;

  case 101:
#line 723 "ncgen.y"
    {
	        (yyval.sym)=(yyvsp[(1) - (1)].sym);
                (yyvsp[(1) - (1)].sym)->is_ref=1;
                (yyvsp[(1) - (1)].sym)->is_prefixed=1;
	        /* path is set in ncgen.l*/
	    ;}
    break;

  case 103:
#line 732 "ncgen.y"
    {;}
    break;

  case 104:
#line 733 "ncgen.y"
    {;}
    break;

  case 107:
#line 741 "ncgen.y"
    {(yyvsp[(1) - (3)].sym)->data = (yyvsp[(3) - (3)].datalist);;}
    break;

  case 108:
#line 745 "ncgen.y"
    {(yyval.datalist) = builddatalist(0); dlappend((yyval.datalist),&((yyvsp[(1) - (1)].constant)));;}
    break;

  case 109:
#line 746 "ncgen.y"
    {(yyval.datalist)=(yyvsp[(1) - (3)].datalist); dlappend((yyvsp[(1) - (3)].datalist),&((yyvsp[(3) - (3)].constant)));;}
    break;

  case 110:
#line 750 "ncgen.y"
    {(yyval.constant)=(yyvsp[(1) - (1)].constant);;}
    break;

  case 111:
#line 751 "ncgen.y"
    {(yyval.constant)=builddatasublist((yyvsp[(2) - (3)].datalist));;}
    break;

  case 112:
#line 755 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_CHAR);;}
    break;

  case 113:
#line 756 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_BYTE);;}
    break;

  case 114:
#line 757 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_SHORT);;}
    break;

  case 115:
#line 758 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_INT);;}
    break;

  case 116:
#line 759 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_INT64);;}
    break;

  case 117:
#line 760 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_UBYTE);;}
    break;

  case 118:
#line 761 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_USHORT);;}
    break;

  case 119:
#line 762 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_UINT);;}
    break;

  case 120:
#line 763 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_UINT64);;}
    break;

  case 121:
#line 764 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_FLOAT);;}
    break;

  case 122:
#line 765 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_DOUBLE);;}
    break;

  case 123:
#line 766 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_STRING);;}
    break;

  case 124:
#line 767 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_OPAQUE);;}
    break;

  case 125:
#line 768 "ncgen.y"
    {(yyval.constant)=makeenumconst((yyvsp[(1) - (1)].sym));;}
    break;

  case 126:
#line 769 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_FILLVALUE);;}
    break;

  case 127:
#line 773 "ncgen.y"
    {(yyval.datalist) = builddatalist(0); dlappend((yyval.datalist),&((yyvsp[(1) - (1)].constant)));;}
    break;

  case 128:
#line 774 "ncgen.y"
    {(yyval.datalist)=(yyvsp[(1) - (3)].datalist); dlappend((yyvsp[(1) - (3)].datalist),&((yyvsp[(3) - (3)].constant)));;}
    break;

  case 129:
#line 778 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_INT);;}
    break;

  case 130:
#line 782 "ncgen.y"
    {(yyval.constant)=makedataconst(NC_STRING);;}
    break;


/* Line 1267 of yacc.c.  */
#line 2575 "ncgen.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 787 "ncgen.y"


#ifndef NO_STDARG
static void
yyerror(const char *fmt, ...)
#else
static void
yyerror(fmt,va_alist) const char* fmt; va_dcl
#endif
{
    va_list argv;
    vastart(argv,fmt);
    (void)fprintf(stderr,"%s: %s line %d: ", progname, cdlname, lineno);
    vderror(fmt,argv);
}

/* undefine yywrap macro, in case we are using bison instead of yacc */
#ifdef yywrap
#undef yywrap
#endif

static int
yywrap(void)                    /* returns 1 on EOF if no more input */
{
    return  1;
}

/* get lexical input routine generated by lex  */
#include "ncgenyy.c"

/* Really should init our data within this file */
void
parse_init(void)
{
    int i;
    derror_count=0;
    opaqueid = 0;
    arrayuid = 0;
    symlist = NULL;
    stack = listnew();
    groupstack = listnew();
    consttype = NC_NAT;
    grpdefs = listnew();
    dimdefs = listnew();
    attdefs = listnew();
    gattdefs = listnew();
    xattdefs = listnew();
    typdefs = listnew();
    vardefs = listnew();
    condefs = listnew();
    tmp = listnew();
    createrootgroup();
    /* Create the primitive types */
    for(i=NC_NAT+1;i<=NC_STRING;i++) {
        primsymbols[i] = makeprimitivetype(i);
    }
    lex_init();
}

static Symbol*
makeprimitivetype(nc_type nctype)
{
    Symbol* sym = install(primtypenames[nctype]);
    sym->objectclass=NC_TYPE;
    sym->subclass=NC_PRIM;
    sym->ncid = nctype;
    sym->typ.basetype = NULL;
    sym->typ.typecode = nctype;
    sym->typ.size = ncsize(nctype);
    sym->typ.alignment = nctypealignment(nctype);
    sym->prefix = listnew();
    return sym;
}

/* Symbol table operations for ncgen tool */
/* install sname in symbol table */
Symbol*
install(const char *sname)
{
    Symbol* sp;
    sp = (Symbol*) emalloc (sizeof (struct Symbol));
    memset((void*)sp,0,sizeof(struct Symbol));    
    sp->name = nulldup(sname);
    sp->next = symlist;
    sp->lineno = lineno;
    sp->location = currentgroup();
    sp->container = currentgroup();
    symlist = sp;
    return sp;
}


static void
setunlimiteddim(Symbol* udim)
{
    rootgroup->grp.unlimiteddim = udim;
}

static Symbol*
getunlimiteddim(void)
{
    return rootgroup->grp.unlimiteddim;
}

static Symbol*
currentgroup(void)
{
    if(listlength(groupstack) == 0) return rootgroup;
    return (Symbol*)listtop(groupstack);
}

static Symbol*
createrootgroup(void)
{
    Symbol* gsym = install(ROOTGROUPNAME);
    gsym->objectclass = NC_GRP;
    gsym->container = NULL;
    gsym->subnodes = listnew();
    gsym->grp.is_root = 1;
    gsym->grp.unlimiteddim = NULL;
    gsym->prefix = listnew();
    listpush(grpdefs,(elem_t)gsym);
    rootgroup = gsym;
    return gsym;
}

static Symbol*
creategroup(Symbol * gsym)
{
    /* See if this group already exists in currentgroup */
    gsym->objectclass = NC_GRP;
    if(dupobjectcheck(NC_GRP,gsym)) {
        derror("Duplicate group name in same scope: %s",gsym->name);
	return NULL;
    }
    addtogroup(gsym);
    gsym->subnodes = listnew();
    listpush(groupstack,(elem_t)gsym);
    listpush(grpdefs,(elem_t)gsym);
    return gsym;
}

static Constant
makedataconst(nc_type nctype)
{
    Constant con;
    consttype = nctype;
    con.nctype = nctype;
    con.lineno = lineno;
    switch (nctype) {
	case NC_CHAR: con.value.charv = char_val; break;
        case NC_BYTE: con.value.int8v = byte_val; break;
        case NC_SHORT: con.value.int16v = int16_val; break;
        case NC_INT: con.value.int32v = int32_val; break;
        case NC_FLOAT: con.value.floatv = float_val; break;
        case NC_DOUBLE: con.value.doublev = double_val; break;

        case NC_STRING: { /* convert to a set of chars*/
	    int len;
	    len = strlen(lextext);
	    if(len == 0) len = 1;
	    con.value.stringv.len = len;
	    con.value.stringv.stringv = strdup(lextext);
	    } break;

#ifdef USE_NETCDF4
        case NC_UBYTE: con.value.uint8v = ubyte_val; break;
        case NC_USHORT: con.value.uint16v = uint16_val; break;
        case NC_UINT: con.value.uint32v = uint32_val; break;
        case NC_INT64: con.value.int64v = int64_val; break;
        case NC_UINT64: con.value.uint64v = uint64_val; break;
	case NC_OPAQUE: {
	    char* s;
	    int len,padlen;
	    len = strlen(lextext);
	    padlen = len;
	    if(padlen < 16) padlen = 16;
	    if((padlen % 2) == 1) padlen++;
	    s = (char*)emalloc(padlen+1);
	    memset((void*)s,'0',padlen);
	    s[padlen]='\0';
	    strncpy(s,lextext,len);
	    con.value.opaquev.stringv = s;
	    con.value.opaquev.len = padlen;
	    } break;
#endif

	case NC_FILLVALUE:
	    break; /* no associated value*/
	    
	default:
	    yyerror("Data constant: unexpected NC type: %s",
		    nctypename(nctype));
	    con.value.stringv.stringv = NULL;    
	    con.value.stringv.len = 0;
    }
    return con;
}

static Constant
makeenumconst(Symbol* econst)
{
    Constant con;
    if(usingclassic) {
        verror("Illegal type: enum");
    } 
    consttype = NC_ENUM;
    con.nctype = NC_ECONST;
    con.lineno = lineno;
    /* fix up econst to be a ref to an econst*/
    econst->objectclass = NC_TYPE;
    econst->subclass = NC_ECONST;
    {
	Symbol* defsym;
	defsym = locate(econst);
	if(defsym == NULL)
	    derror("Undefined or forward referenced enum constant: %s",econst->name);
	econst = defsym;
    }
    con.value.enumv = econst;
    return con;
}

static void
addtogroup(Symbol* sym)
{
    Symbol* grp = currentgroup();
    sym->container = grp;
    listpush(grp->subnodes,(elem_t)sym);
    setpathcurrent(sym);
}

/* Check for duplicate name of given type within current group*/
static int
dupobjectcheck(nc_class objectclass, Symbol* pattern)
{
    int i;
    Symbol* grp;
    if(pattern == NULL) return 0;
    grp = pattern->container;
    if(grp == NULL || grp->subnodes == NULL) return 0;
    for(i=0;i<listlength(grp->subnodes);i++) {
	Symbol* sym = (Symbol*)listget(grp->subnodes,i);
	if(!sym->is_ref && sym->objectclass == objectclass
	   && strcmp(sym->name,pattern->name)==0) return 1;
    }
    return 0;
}

static void
setpathcurrent(Symbol* sym)
{
    sym->is_prefixed = 0;
    sym->prefix = prefixdup(groupstack);
}

/* Convert an nc_type code to the corresponding Symbol*/
Symbol*
basetypefor(nc_type nctype)
{
    return primsymbols[nctype];
}

static char*
primtypename(nc_type nctype)
{
    char* tmp;
    if(isprim(nctype)) return primtypenames[nctype];
    if(nctype == NC_OPAQUE) return "opaque";
    if(nctype == NC_ENUM) return "enum";
    if(nctype == NC_FILLVALUE) return "_";
    tmp = poolalloc(64);
    sprintf(tmp,"nc_<%d>",nctype);
    return tmp;
}

static char*
specialname(int tag)
{
    switch (tag) {
    case _FILLVALUE: return "FillValue";
    case _FORMAT: return "Format";
    case _STORAGE: return "Storage";
    case _CHUNKSIZES: return "ChunSizes";
    case _FLETCHER32: return "Fletcher32";
    case _DEFLATELEVEL: return "DeflateLevel";
    case _SHUFFLE: return "Shuffle";
    case _ENDIANNESS: return "Endianness";
    case _NOFILL: return "NoFill";
    default: break;
    }
    return "<unknown>";
}

static Symbol*
makespecial(int tag, Symbol* vsym, Symbol* tsym, void* data)
{
    Symbol* attr = NULL;
    Datalist* list = (Datalist*)data;
    Specialdata* special;
    Constant* con = (Constant*)data;
    char* sdata = NULL;
    int i;

    if(tag == _FORMAT && vsym != NULL) {
	derror("_Format: must be global attribute");
	vsym = NULL;
    }

    if(con->nctype == NC_STRING) {
	sdata = con->value.stringv.stringv;
    }

    if(vsym != NULL) special = &vsym->var.special;
    if(tag == _FORMAT) {
	if(strcasecmp(sdata,"classic") == 0) {
	    cmode_modifier = 0;
	} else if(strcasecmp(sdata,"64-bit offset") == 0) {
	    cmode_modifier = NC_64BIT_OFFSET;
	} else if(strcasecmp(sdata,"netcdf-4") == 0) {
	    if(usingclassic) verror("_Format: non-classic mode value: %s",sdata);
	    cmode_modifier = NC_NETCDF4;
	} else if(strcasecmp(sdata,"netcdf-4 classic model") == 0) {
	    if(usingclassic) verror("_Format: non-classic mode value: %s",sdata);
	    cmode_modifier = NC_NETCDF4 | NC_CLASSIC_MODEL;
	} else
	    derror("_Format: illegal value: %s",sdata);
        /* Recompute mode flag*/
        usingclassic = ((cmode_modifier & NC_NETCDF4) == 0);
    } else if(tag == _FILLVALUE) {
	special->_Fillvalue = list;
	/* fillvalue must be a single value*/
	if(list->length != 1)
	    derror("_FillValue: must be a single (possibly compound) value",
			vsym->name);
        /* check that the attribute value contains no fill values*/
        if(containsfills(list)) {
	    derror("Attribute data may not contain fill values (i.e. _ )");
        }
	/* _FillValue is also a real attribute*/
	if(vsym->objectclass != NC_VAR) {
	    derror("_FillValue attribute not associated with variable: %s",vsym->name);
	}
	if(tsym  == NULL) tsym = vsym->typ.basetype;
	else if(vsym->typ.basetype != tsym) {
	    derror("_FillValue attribute type does not match variable type: %s",vsym->name);
	}
	attr=makeattribute(install("_FillValue"),vsym,tsym,list,ATTRVAR);
    } else {
#ifndef USE_NETCDF4
        verror("Special attribute not supported in classic mode: %s",specialname(tag));
#else
        if(usingclassic)
	    verror("Special attribute not supported in classic mode: %s",specialname(tag));
        else switch (tag) {
        case _STORAGE:
            if(strcmp(sdata,"contiguous") == 0)
                special->_Storage = NC_CONTIGUOUS;
            else if(strcmp(sdata,"chunked") == 0)
                special->_Storage = NC_CHUNKED;
            else
                derror("_Storage: illegal value: %s",sdata);
            special->flags |= _STORAGE_FLAG;
            break;
        case _FLETCHER32:
            if(strcmp(sdata,"false") == 0)
                special->_Fletcher32 = 0;
            else if(strcmp(sdata,"true") == 0)
                special->_Fletcher32 = 1;
            else
                derror("_Fletcher32: illegal value: %s",sdata);
            special->flags |= _FLETCHER32_FLAG;
            break;
        case _DEFLATELEVEL:
            if(con->value.int32v >= 0 && con->value.int32v <= 9)
                special->_DeflateLevel = con->value.int32v;
            else
                derror("_DeflateLevel: illegal value: %d",con->value.int32v);
            special->flags |= _DEFLATE_FLAG;
            break;
        case _SHUFFLE:
            if(strcmp(sdata,"false") == 0)
                special->_Shuffle = 0;
            else if(strcmp(sdata,"true") == 0)
                special->_Shuffle = 1;
            else
                derror("_Shuffle: illegal value: %s",sdata);
            special->flags |= _SHUFFLE_FLAG;
            break;
        case _ENDIANNESS:
            if(strcmp(sdata,"little") == 0)
                special->_Endianness = 1;
            else if(strcmp(sdata,"big") == 0)
                special->_Endianness = 2;
            else
                derror("_Endianness: illegal value: %s",sdata);
            special->flags |= _ENDIAN_FLAG;
            break;
        case _NOFILL:
            if(strcmp(sdata,"false") == 0)
                special->_Fill = 1;
            else if(strcmp(sdata,"true") == 0)
                special->_Fill = 0;
            else
                derror("_NoFill: illegal value: %s",sdata);
            special->flags |= _NOFILL_FLAG;
            break;
        case _CHUNKSIZES:
            special->nchunks = list->length;
            special->_ChunkSizes = (size_t*)emalloc(sizeof(size_t)*special->nchunks);
            for(i=0;i<special->nchunks;i++) {
                special->_ChunkSizes[i] = (size_t)list->data[i].value.int32v;
            }
            special->flags |= _CHUNKSIZE_FLAG;
            break;
        default: PANIC1("makespecial: illegal token: %d",tag);
        }
#endif
    }
    return attr;
}

static Symbol*
makeattribute(Symbol* asym,
		Symbol* vsym,
		Symbol* tsym,
		Datalist* data,
		Attrkind kind) /* global var or unknown*/
{
    asym->objectclass = NC_ATT;
    asym->data = data;
    addtogroup(asym);
    switch (kind) {
    case ATTRVAR:
        asym->att.var = vsym;
        asym->typ.basetype = tsym;
        listpush(attdefs,(elem_t)asym);
	break;
    case ATTRGLOBAL:
        asym->att.var = NULL; /* NULL => NC_GLOBAL*/
        asym->typ.basetype = tsym;
        listpush(gattdefs,(elem_t)asym);
	break;
    default: PANIC1("unexpected attribute type: %d",kind);
    }
    /* finally; check that the attribute value contains no fill values*/
    if(containsfills(data)) {
	derror("Attribute data may not contain fill values (i.e. _ ): %s",asym->name);
    }
    return asym;
}

static int
containsfills(Datalist* list)
{
    int i;
    Constant* con = list->data;
    for(i=0;i<list->length;i++,con++) {
	if(con->nctype == NC_COMPOUND) {
	    if(containsfills(con->value.compoundv)) return 1;	
	} else if(con->nctype == NC_FILLVALUE) return 1;	
    }
    return 0;
}

