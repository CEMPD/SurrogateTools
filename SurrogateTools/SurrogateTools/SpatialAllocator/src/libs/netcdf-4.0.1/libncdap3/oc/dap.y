/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

%{
#define YYDEBUG 1
#include "ocinternal.h"
#include "dapparselex.h"
#ifdef USE_DAP
#include "daptab.h"
#else
#include "dap.tab.h"
#endif
extern int yylex (YYSTYPE*, DAPparsestate*);
extern int yyerror(DAPparsestate*,char*);
%}

%pure-parser
%lex-param {DAPparsestate* parsestate}
%parse-param {DAPparsestate* parsestate}

%token SCAN_WORD
%token SCAN_BYTE
%token SCAN_INT16
%token SCAN_UINT16
%token SCAN_INT32
%token SCAN_UINT32
%token SCAN_FLOAT32
%token SCAN_FLOAT64
%token SCAN_STRING
%token SCAN_URL 
%token SCAN_DATASET
%token SCAN_SEQUENCE
%token SCAN_STRUCTURE
%token SCAN_GRID
%token SCAN_ARRAY
%token SCAN_MAPS 
%token SCAN_ATTR
%token SCAN_ALIAS 
%token SCAN_DATABEGIN

/*
%type datasetbody datasetname
%type declarations declaration
%type base_type
%type array_decls array_decl
%type attributebody attr_list attribute
%type bytes int16 uint16 int32 uint32 float32 float64 strs urls url
%type str_or_id
%type var_name name
%type alias
*/

%start start

%%

start:
	  SCAN_DATASET datasetbody
	| SCAN_ATTR attributebody
	;

datasetbody:
	  '{' declarations '}' datasetname ';'
		{$$=datasetbody(parsestate,$4,$2);}
        | error
            {yyerror(parsestate,"Illegal dataset body"); YYABORT;}
	;

declarations:
	  /* empty */ {$$=declarations(parsestate,NULL,NULL);}
        | declarations declaration {$$=declarations(parsestate,$1,$2);}
	;

/* 01/21/08: James say: no dimensions for grids or sequences */
declaration:
	  base_type var_name array_decls ';'
		{$$=makebase(parsestate,$2,$1,$3);}
	| SCAN_STRUCTURE '{' declarations '}' var_name array_decls ';'
	    {$$=makestructure(parsestate,$5,$6,$3);}
	| SCAN_SEQUENCE '{' declarations '}' var_name ';'
	    {$$=makesequence(parsestate,$5,$3);}
	| SCAN_GRID '{' SCAN_ARRAY ':' declaration SCAN_MAPS ':' declarations '}' var_name ';'
	    {$$=makegrid(parsestate,$10,$5,$8);}
        | error 
            {yyerror(parsestate,"Unrecognized type"); YYABORT;}
	;
 

base_type:
	  SCAN_BYTE {$$=(Object)OC_Byte;}
	| SCAN_INT16 {$$=(Object)OC_Int16;}
	| SCAN_UINT16 {$$=(Object)OC_UInt16;}
	| SCAN_INT32 {$$=(Object)OC_Int32;}
	| SCAN_UINT32 {$$=(Object)OC_UInt32;}
	| SCAN_FLOAT32 {$$=(Object)OC_Float32;}
	| SCAN_FLOAT64 {$$=(Object)OC_Float64;}
	| SCAN_URL {$$=(Object)OC_URL;}
	| SCAN_STRING {$$=(Object)OC_String;}
	;

array_decls:
	  /* empty */ {$$=arraydecls(parsestate,NULL,NULL);}
	| array_decls array_decl {$$=arraydecls(parsestate,$1,$2);}
	;

array_decl:
	   '[' SCAN_WORD ']' {$$=arraydecl(parsestate,NULL,$2);}
	 | '[' name '=' SCAN_WORD ']' {$$=arraydecl(parsestate,$2,$4);}
	 | error
	    {yyerror(parsestate,"Illegal dimension declaration"); YYABORT;}
	;

datasetname:
	  var_name {$$=$1;}
	| SCAN_DATASET {$$=strdup("dataset");}//"dataset" is legal datasetname
        | error
	    {yyerror(parsestate,"Illegal dataset declaration"); YYABORT;}
	;

var_name: name {$$=$1;};

attributebody:
	  '{' attr_list '}' {$$=attributebody(parsestate,$2);}
	| error
            {yyerror(parsestate,"Illegal DAS body"); YYABORT;}
	;

attr_list:
	  /* empty */ {$$=attrlist(parsestate,NULL,NULL);}
	| attr_list attribute {$$=attrlist(parsestate,$1,$2);}
	;

attribute:
	  alias {$$=NULL;} /* not implemented */ 
        | SCAN_BYTE name bytes ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_Byte);}
	| SCAN_INT16 name int16 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_Int16);}
	| SCAN_UINT16 name uint16 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_UInt16);}
	| SCAN_INT32 name int32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_Int32);}
	| SCAN_UINT32 name uint32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_UInt32);}
	| SCAN_FLOAT32 name float32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_Float32);}
	| SCAN_FLOAT64 name float64 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_Float64);}
	| SCAN_STRING name strs ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_String);}
	| SCAN_URL name urls ';'
	    {$$=attribute(parsestate,$2,$3,(Object)OC_URL);}
	| name '{' attr_list '}' {$$=attrset(parsestate,$1,$3);}
	| error 
            {yyerror(parsestate,"Illegal attribute"); YYABORT;}
	;

bytes:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_Byte);}
	| bytes ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)OC_Byte);}
	;
int16:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_Int16);}
	| int16 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)OC_Int16);}
	;
uint16:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_UInt16);}
	| uint16 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)OC_UInt16);}
	;
int32:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_Int32);}
	| int32 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)OC_Int32);}
	;
uint32:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_UInt32);}
	| uint32 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)OC_UInt32);}
	;
float32:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_Float32);}
	| float32 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)OC_Float32);}
	;
float64:
	  SCAN_WORD {$$=attrvalue(parsestate,NULL,$1,(Object)OC_Float64);}
	| float64 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)OC_Float64);}
	;
strs:
	  str_or_id {$$=attrvalue(parsestate,NULL,$1,(Object)OC_String);}
	| strs ',' str_or_id {$$=attrvalue(parsestate,$1,$3,(Object)OC_String);}
	;

urls:
	  url {$$=attrvalue(parsestate,NULL,$1,(Object)OC_URL);}
	| urls ',' url {$$=attrvalue(parsestate,$1,$3,(Object)OC_URL);}
	;

url:
	SCAN_WORD {$$=$1;}
	;

str_or_id:
	SCAN_WORD {$$=$1;}
	;

/* Not used
float_or_int:
	SCAN_WORD {$$=$1;}
	;
*/

alias:
	SCAN_ALIAS SCAN_WORD SCAN_WORD
	    {yyerror(parsestate,"Alias not currently supported"); YYABORT;}
	;

/* Note that variable names like "byte" are legal names
   and are disambiguated by context
*/
name:
          SCAN_WORD {$$=$1;}
	| SCAN_BYTE {$$=$1;}
	| SCAN_INT16 {$$=$1;}
	| SCAN_INT32 {$$=$1;}
	| SCAN_UINT16 {$$=$1;}
	| SCAN_UINT32 {$$=$1;}
	| SCAN_FLOAT32 {$$=$1;}
	| SCAN_FLOAT64 {$$=$1;}
	| SCAN_STRING {$$=$1;}
	| SCAN_URL {$$=$1;}
	| SCAN_STRUCTURE {$$=$1;}

	| SCAN_SEQUENCE {$$=$1;}
	| SCAN_GRID {$$=$1;}
	| SCAN_ARRAY {$$=$1;}
	| SCAN_MAPS {$$=$1;}
	| SCAN_ATTR {$$=$1;}
	;

%%
