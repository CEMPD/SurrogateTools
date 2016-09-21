## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This file builds and runs the nc_test program, which tests the
# netCDF-3 API for all formats.

# $Id: Makefile.am,v 1.5 2009/03/22 03:30:25 dmh Exp $

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/ncdump

# Link to the netCDF-4 and the HDF5 libraries.

if BUILD_DAP

if USE_NETCDF4

LDADD = ../libsrc4/libnetcdf.la -lhdf5_hl -lhdf5 

# Point pre-processor and linker to netcdf-4 directory (libsrc4).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc4
AM_LDFLAGS = -L../libsrc4

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root location for ZLIB, use it.
if USE_ZLIB_DIR
AM_CPPFLAGS += -I@ZLIBDIR@/include
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_SZLIB_DIR
AM_CPPFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

else # ! USE_NETCDF4

LDADD = ${top_builddir}/libncdap3/libnetcdf.la

endif # USE_NETCDF4

endif # BUILD_DAP

# Set up the tests.
TESTS = tst_ncdap3.sh

# remote tests are optional
# because the server may be down
if ENABLE_DAP_REMOTE_TESTS
TESTS += tst_ncdap3_remote.sh
endif

# Need to add subdirs
SUBDIRS=expected3 testdata3

EXTRA_DIST = tst_ncdap3.sh tst_ncdap3_remote.sh tst_ncdap3_shared.sh 

# Only clean these on mainatiner-clean, because they require m4 and
# automake to regenerate.
MAINTAINERCLEANFILES = Makefile.in
CLEANFILES=results/*.dmp results/*.das results/*.dds

test: check

