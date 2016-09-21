#!/bin/sh

# This test set is similar to runtests,
# but its goal is to cycle the ncdump output thru ncgen4
# to look for syntax mismatches between ncgen4 and ncdump

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

if test "${KFLAG}" = 1 ; then
TESTSET="${TESTS3} ${BIGTESTS3} ${BIGBIG3}"
else
TESTSET="${TESTS3} ${TESTS4} ${BIGTESTS} ${SPECIALTESTS}"
fi

echo "*** Testing ncdump to ncgen4 to ncdump cycle"

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
	for t in ${XFAILTESTS} ; do
	if test "x${t}" = "x${x}" ; then isxfail=1; fi
	done
  rm -f ${x}.nc ${x}.dmp
  # step 1: use original cdl to build the .nc
  ${builddir}/ncgen4/ncgen4 -k${KFLAG} -o ${x}.nc ${cdl}/${x}.cdl
  # step 2: dump .nc file
  ${builddir}/ncdump/ncdump ${specflag} ${x}.nc > ${x}.dmp
  # step 3: use ncdump output to (re-)build the .nc
  rm -f ${x}.nc
  ${builddir}/ncgen4/ncgen4 -k${KFLAG} -o ${x}.nc ${x}.dmp
  # step 4: dump .nc file again
  ${builddir}/ncdump/ncdump ${specflag} ${x}.nc > ${x}.dmp2
  # compare the two ncdump outputs
  if diff -w ${x}.dmp ${x}.dmp2 ; then
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
