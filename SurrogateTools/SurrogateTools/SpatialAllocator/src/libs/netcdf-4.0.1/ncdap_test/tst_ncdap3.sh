#!/bin/sh

# if this is part of a distcheck action, then this script
# will be executed in a different directory
# than the one containing it; so capture the path to this script
# as the location of the source directory.
srcdir=`dirname $0`
tmp=`echo ${srcdir}|sed -e 's/^\\\\//g'`
if test "${tmp}" = "${srcdir}" ; then
  srcdir=`pwd`/${srcdir}
  tmp=`echo ${srcdir}|sed -e 's/\\\\$//g'`
  srcdir=${tmp}
fi

# compute the build directory
# Do a hack to remove e.g. c: for CYGWIN
cd `pwd`
builddir=`pwd`/..

# Hack for CYGWIN
cd $srcdir
srcdir=`pwd`
cd ${builddir}/ncdap_test

RESULTSDIR="./results"

# Locate some tools
NCDUMP="${builddir}/ncdump/ncdump"

# Locate the testdata and expected directory
testdata3="${srcdir}/testdata3"
expected3="${srcdir}/expected3"

# get the list of test files
. ${srcdir}/tst_ncdap3_shared.sh

echo "*** Testing libncap3."

if test -d "${RESULTSDIR}" ; then ignore=1; else mkdir "${RESULTSDIR}"; fi

passcount=0
xfailcount=0
failcount=0

cd ${RESULTSDIR}
for x in ${TESTSET} ; do
  url="${TESTURLPREFIX}/$x"
  echo "*** Testing: ${x}"
	# determine if this is an xfailtest
	isxfail=
	for t in ${XFAILTESTS} ; do
	if test "x${t}" = "x${x}" ; then isxfail=1; fi
	done
  ${NCDUMP} ${url} > ${x}.dmp
  # compare with expected
  if diff -w ${expected3}/${x}.dmp ${x}.dmp ; then
    echo "*** SUCCEED: ${x}"
    passcount=`expr $passcount + 1`
  elif test "x${isxfail}" = "x1" ; then
    echo "*** XFAIL: ${x}"
    xfailcount=`expr $xfailcount + 1`
  else
    echo "*** FAIL: ${x}"
    failcount=`expr $failcount + 1`
  fi
done
cd ..

totalcount=`expr $passcount + $failcount + $xfailcount`
okcount=`expr $passcount + $xfailcount`

echo "*** PASSED: ${okcount}/${totalcount} ; ${xfailcount} expected failures ; ${failcount} unexpected failures"

if test "$failcount" -gt 0 ; then
  exit 1
else
  exit 0
fi
