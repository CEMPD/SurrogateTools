/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SCAN_WORD = 258,
     SCAN_BYTE = 259,
     SCAN_INT16 = 260,
     SCAN_UINT16 = 261,
     SCAN_INT32 = 262,
     SCAN_UINT32 = 263,
     SCAN_FLOAT32 = 264,
     SCAN_FLOAT64 = 265,
     SCAN_STRING = 266,
     SCAN_URL = 267,
     SCAN_DATASET = 268,
     SCAN_SEQUENCE = 269,
     SCAN_STRUCTURE = 270,
     SCAN_GRID = 271,
     SCAN_ARRAY = 272,
     SCAN_MAPS = 273,
     SCAN_ATTR = 274,
     SCAN_ALIAS = 275,
     SCAN_DATABEGIN = 276
   };
#endif
/* Tokens.  */
#define SCAN_WORD 258
#define SCAN_BYTE 259
#define SCAN_INT16 260
#define SCAN_UINT16 261
#define SCAN_INT32 262
#define SCAN_UINT32 263
#define SCAN_FLOAT32 264
#define SCAN_FLOAT64 265
#define SCAN_STRING 266
#define SCAN_URL 267
#define SCAN_DATASET 268
#define SCAN_SEQUENCE 269
#define SCAN_STRUCTURE 270
#define SCAN_GRID 271
#define SCAN_ARRAY 272
#define SCAN_MAPS 273
#define SCAN_ATTR 274
#define SCAN_ALIAS 275
#define SCAN_DATABEGIN 276




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



