# $Id: tst_ncdap3_shared.sh,v 1.9 2009/03/22 03:30:26 dmh Exp $

##################################################
# Local test info
##################################################

# Define the complete URLS

TESTURLPREFIX="[unlimitedsequence]file://${testdata3}"

XFAILTESTS=

SYNTHETICDATA="synth1 synth2 synth3 synth4 synth5 synth6"

ACTUALDATA=\
"1990-S1700101.HDF.WVC_Lat 1998-6-avhrr.dat \
b31a b31 D1 Drifters \
EOSDB ingrid nestedDAS NestedSeq2 \
NestedSeq OverideExample pbug0001b SimpleDrdsExample \
test.01 test.02 test.03 test.04 \
test.05 test.06a test.06 test.07a \
test.07 test.21 test.22 \
test.23 test.31 test.32 \
test.50 test.53 test.55 \
test.56 test.57 \
test.66 test.67 test.68 test.69 \
test.an1 \
test.dfp1 test.dfr1 test.dfr2 test.dfr3 \
test.gr1 test.gr2 test.gr3 test.gr4 \
test.gr5 test.PointFile test.sds1 \
test.sds2 test.sds3 test.sds4 test.sds5 \
test.sds6 test.sds7 test.SwathFile test.vs1 \
test.vs2 test.vs3 test.vs4 test.vs5 whoi"

# Following are in the repository, but
# are too large to include in the distribution
TOOLARGE=test.GridFile

TESTSET="${SYNTHETICDATA} ${ACTUALDATA} ${XFAILTESTS}"

##################################################
# Remote test info
##################################################

TESTSERVER="http://test.opendap.org"
TESTFF="${TESTSERVER}/opendap/nph-dods/data/ff"
TESTHDF="${TESTSERVER}/opendap/nph-dods/data/hdf"

# Client parameters
cl_10="[limit=10]"
cl_gso_13="[limit-GSO_AVHRR=13]"
cl_gso_27="[limit-GSO_AVHRR=27]"
cl_day_7="[limit-GSO_AVHRR.day_num=7]"

# Constraints

ce_day="GSO_AVHRR.day_num"
ce_day_160="GSO_AVHRR.day_num&GSO_AVHRR.day_num>160"

# ce3: an array inside a structure;  watch escaping
ce_nscat_lat="NSCAT Rev 17.WVC_Lat"

# ce4 a field in a structure in a sequence in a structure.
ce_nscat1="NSCAT Rev 17.SwathIndex.begin.begin__0"
ce_nscat2="NSCAT Rev 17.SwathIndex.begin.begin__0&NSCAT Rev 17.SwathIndex.begin.begin__0>0"

avhrr_server="${TESTFF}/1998-6-avhrr.dat"
hdf_server="${TESTHDF}/1990-S1700101.HDF"
nscat_server="${hdf_server}"

avhrr_server_1="${cl_10}$avhrr_server"
avhrr_server_2="${cl_gso_27}$avhrr_server"
avhrr_server_3="${cl_gso_27}${cl_day_7}$avhrr_server"
avhrr_server_4="${cl_gso_13}$avhrr_server"

REMOTETESTS=
XFAILREMOTETESTS=

REMOTETESTSET="${REMOTETESTS} ${XFAILREMOTETESTS}"
