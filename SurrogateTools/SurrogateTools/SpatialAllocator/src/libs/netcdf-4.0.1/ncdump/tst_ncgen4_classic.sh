#!/bin/sh

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

CLASSIC=1 ; export CLASSIC

echo "*** Testing ncgen4 with -k1"

. ${srcdir}/tst_ncgen4.sh

exit