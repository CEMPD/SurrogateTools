#!/bin/sh

echo ${CLASSIC}

# if this is part of a distcheck action, then this script
# will be executed in a different directory
# than the one containing it; so capture the path to this script
# as the location of the source directory.
srcdir=`dirname $0`
tmp=`echo ${srcdir}|sed -e 's/^\\\\//g'`
if test ${tmp} = ${srcdir} ; then
  srcdir=`pwd`/${srcdir}
  tmp=`echo ${srcdir}|sed -e 's/\\\\$//g'`
  srcdir=${tmp}
fi
#echo "srcdir=${srcdir}"

. ${srcdir}/tst_ncgen4_shared.sh

ALLXFAIL="${XFAILTESTS} ${SPECIALTESTS}"

if test "${KFLAG}" = 1 ; then
TESTSET="${TESTS3}"
else
TESTSET="${TESTS3} ${TESTS4} ${ALLXFAIL}"
fi

echo "*** Testing ncgen4."

cd ${RESULTSDIR}
for x in ${TESTSET} ; do
  echo "*** Testing: ${x}"
	# determine if we need the specflag set
	specflag=
	for s in ${SPECIALTESTS} ; do
	if test "x${s}" = "x${x}" ; then specflag="-s"; fi
	done
	# determine if this is an xfailtest
	isxfail=
	for t in ${ALLXFAIL} ; do
	if test "x${t}" = "x${x}" ; then isxfail=1; fi
	done
  ${builddir}/ncgen4/ncgen4 -k${KFLAG} -o ${x}.nc ${cdl}/${x}.cdl
  # dump .nc file
  ${builddir}/ncdump/ncdump ${specflag} ${x}.nc > ${x}.dmp
  # compare with expected
  if diff -w ${expected}/${x}.dmp ${x}.dmp ; then
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

if test $failcount -gt 0 ; then
  exit 1
else
  exit 0
fi
