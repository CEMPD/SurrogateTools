## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This automake file generates the Makefile to build netCDF-4. The
# generated makefile is not run unless the user selected to build
# netCDF-4.

# $Id: Makefile.am,v 1.16 2009/03/30 17:03:55 dmh Exp $

SUBDIRS=oc

AM_CPPFLAGS = -I$(top_srcdir)/libsrc

# Support later += operations
libnetcdf_la_LIBADD =
AM_LDFLAGS = 

CURL_CFLAGS = @CURL_CFLAGS@

# DRNO Sources
SRC=\
dapcvt.c \
dapalign.c \
dapodom.c \
getvar3.c \
ncdaperr.c \
cdf3.c \
ncdap3.c \
dapdispatch3.c \
dispatch3.c \
dapdump.c \
dapdebug.c \
daputil.c \
nchashmap.c nclist.c ncbytebuffer.c

HDRS=\
ncdap.h \
ncdap3.h \
daprename.h \
dapalign.h \
dapodom.h \
getvar3.h \
dapnc.h \
daputil.h \
dispatch3.h \
dispatchdefs3.h \
dapdispatch3.h \
dapdebug.h \
dapdump.h \
nchashmap.h nclist.h ncbytebuffer.h

if BUILD_DAP

# Our output library will include contents of the netcdf3
# convenience library and optionally the liboc library

libnetcdf_la_SOURCES = $(SRC) $(HDRS)
libnetcdf_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)

if INTERNAL_OCLIB
  AM_CPPFLAGS += -I${top_srcdir}/libncdap3/oc
  libnetcdf_la_LIBADD += ./oc/liboc.la
else
  AM_LDFLAGS += -L@OCLIBDIR@/lib
  libnetcdf_la_LIBADD += -loc
endif # INTERNAL_OCLIB

libnetcdf_la_LIBADD += ../libsrc/libnetcdf3.la
if USE_NETCDF4
noinst_LTLIBRARIES = libnetcdf.la
else
lib_LTLIBRARIES = libnetcdf.la
endif

# Include other libraries only if we are the terminal library
if !USE_NETCDF4

# Include netcdf-2 unless v2 has been turned off..
if BUILD_V2
libnetcdf_la_LIBADD += ../libsrc/libnetcdf2.la
endif

# If we are not building separate fortran libraries, then include
# their functions in the C library.
if !BUILD_SEPARATE_FORTRAN

# If F77 is built, include its convenience library. If F90 is built,
# it will also be part of this convenience library.
if BUILD_F77
libnetcdf_la_LIBADD += ../fortran/libnetcdff.la
endif

endif # !BUILD_SEPARATE_FORTRAN

endif

endif # BUILD_DAP

# Clean up. Each test program creates one data file, with the same
# name as the test program, and either a .h5 or .nc, for "pure" HDF5
# files and netCDF-4 files.
MAINTAINERCLEANFILES = Makefile.in
CLEANFILES =
DISTCLEANFILES =

test: check

