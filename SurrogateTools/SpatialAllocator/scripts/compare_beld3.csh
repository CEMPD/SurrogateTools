#!/bin/csh -f
#******************* Beld3smk Run Script *************************************
# Runs beld3smk for sample modeling grid
# July 2005, CAS 
#*****************************************************************************

setenv DEBUG_OUTPUT N 

# Set executable
setenv MIMSDIR ..
setenv EXE $MIMSDIR/diffioapi.exe

# Set Input Directory
setenv DATADIR $MIMSDIR/data
setenv OUTPUT  $MIMSDIR/output

# set script variables
set grid = M_08_99NASH
set eflag = 0

# Set program parameters
setenv TOLERANCE 0.01

# compare "a" file 
setenv ORIG_FILE $DATADIR/beld3.${grid}.output_a.ncf
setenv NEW_FILE $OUTPUT/beld3_${grid}_output_a.ncf

echo "Comparing $NEW_FILE"
echo "to $ORIG_FILE"

$EXE 

if($status != 0) then
  echo "ERROR comparing beld3.${grid}.output_a.ncf files."
  set eflag = 1
endif

# compare "b" file
setenv ORIG_FILE $DATADIR/beld3.${grid}.output_b.ncf
setenv NEW_FILE $OUTPUT/beld3_${grid}_output_b.ncf

echo "Comparing $NEW_FILE"
echo "to $ORIG_FILE"

$EXE 

if($status != 0) then
  echo "ERROR comparing beld3.${grid}.output_b.ncf files."
  set eflag = 1
endif

# compare "tot" file
setenv ORIG_FILE $DATADIR/beld3.${grid}.output_tot.ncf
setenv NEW_FILE $OUTPUT/beld3_${grid}_output_tot.ncf

echo "Comparing $NEW_FILE"
echo "to $ORIG_FILE"

$EXE 

if($status != 0) then
  echo "ERROR comparing beld3.${grid}.output_tot.ncf files."
  set eflag = 1
endif

if($eflag == 1) then
  echo "Test suite failed!"
else
  echo "Test suite successful!"
endif

exit(0)

