## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This automake file generates the Makefile to build netCDF-4. The
# generated makefile is not run unless the user selected to build
# netCDF-4.

# $Id: Makefile.am,v 1.9 2009/03/25 15:17:20 dmh Exp $

# Cause C preprocessor to search current and parent directory.
AM_CPPFLAGS = -I$(top_srcdir)/libsrc

# Linker needs to search current directory when building tests.
AM_LDFLAGS = 

CURL_CFLAGS = @CURL_CFLAGS@
CURL_LIBS = @CURL_LIBS@

# OC Sources
SRC=\
ocxdr_stdio.c \
occompile.c \
occontent.c \
ocdata.c \
oc.c \
ocnode.c \
read.c \
http.c \
dapparselex.c \
dapurl.c \
ocutil.c \
ocdebug.c \
ocdump.c \
oclog.c \
ocdrno.c \
rc.c \
curlfunctions.c \
oclist.c \
ocbytes.c

HDRS=\
occontent.h \
ocdata.h \
oc.h \
ocinternal.h \
ocnode.h \
ocdatatypes.h \
dapurl.h \
constraints.h \
read.h \
http.h \
dapparselex.h \
ocutil.h \
ocdebug.h \
ocdump.h \
oclog.h \
ocdrno.h \
rc.h \
oclist.h \
ocbytes.h

GRAMFILES=dap.y
# autoconf persists in removing the bison output unless it is renames
GRAMC=daptab.c
GRAMH=daptab.h

if BUILD_DAP
noinst_LTLIBRARIES = liboc.la
# Watch out; the gram files must be first
liboc_la_SOURCES = ${GRAMC} ${GRAMH} $(SRC) $(HDRS) 
liboc_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
liboc_la_LIBADD = $(CURL_LIBS)
endif

EXTRA_DIST=${GRAMFILES}
MAINTAINERCLEANFILES = Makefile.in ${GRAMC} ${GRAMH}
CLEANFILES =
DISTCLEANFILES =

#Normally not invoked unless after maintainer-clean or deliberate deletion
daptab.c daptab.h: dap.y
	rm -f dap.tab.c dap.tab.h
	bison -d -p dap dap.y
	mv dap.tab.c daptab.c; mv dap.tab.h daptab.h

test: check

