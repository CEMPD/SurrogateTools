#!/bin/csh -f
#******************* Beld3smk Run Script *************************************
# Runs beld3smk for sample modeling grid
# June 2006, LR 
# Modified Dec. 11, 2007
#*****************************************************************************

setenv DEBUG_OUTPUT Y 

# Set executable
setenv MIMSDIR ..
setenv EXE $MIMSDIR/beld3smk.exe
setenv ALLOCATOR_EXE $MIMSDIR/allocator.exe

# Set Input Directory
setenv DATADIR $MIMSDIR/data

# Set output Directory -- the directory has to exist!
setenv OUTPUT  $MIMSDIR/output

setenv TIME time

# Set program parameters
setenv OUTPUT_GRID_NAME M_08_99NASH
setenv OUTPUT_FILE_TYPE RegularGrid
setenv OUTPUT_FILE_ELLIPSOID "+a=6370997.00,+b=6370997.00"
setenv INPUT_DATA_DIR $DATADIR/beld/
setenv GRIDDESC $INPUT_DATA_DIR/GRIDDESC.txt
setenv TMP_DATA_DIR $MIMSDIR/tmp/
setenv OUTPUT_FILE_PREFIX $OUTPUT/beld3_${OUTPUT_GRID_NAME}_output

# Create temporary data directory if needed
if(! -e $TMP_DATA_DIR) mkdir $TMP_DATA_DIR

$TIME $EXE 

