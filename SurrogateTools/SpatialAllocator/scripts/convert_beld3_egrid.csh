#!/bin/csh -f
#******************* Beld3smk Run Script *************************************
# Runs beld3smk for sample modeling grid
# Mar 2006, LR 
# Dec. 2007, LR
#*****************************************************************************

setenv DEBUG_OUTPUT Y 

# Set executable
setenv MIMSDIR ..
setenv EXE $MIMSDIR/beld3smk.exe
setenv ALLOCATOR_EXE $MIMSDIR/allocator.exe

# Set Input Directory
setenv DATADIR $MIMSDIR/data

# Set output directory -- the directory has to exist
setenv OUTPUT  $MIMSDIR/output

setenv TIME time

# Set program parameters
setenv OUTPUT_GRID_NAME EGRID_359X573
setenv OUTPUT_FILE_TYPE EGrid
setenv OUTPUT_FILE_ELLIPSOID "+a=6371200.0,+b=6371200.0"
setenv OUTPUT_POLY_FILE $DATADIR/egrid_arc.dat
setenv INPUT_DATA_DIR $DATADIR/beld/
setenv GRIDDESC $INPUT_DATA_DIR/GRIDDESC.txt
setenv TMP_DATA_DIR $MIMSDIR/tmp/
setenv OUTPUT_FILE_PREFIX $OUTPUT/beld3_${OUTPUT_GRID_NAME}_output

# Create temporary data directory if needed
if(! -e $TMP_DATA_DIR) mkdir $TMP_DATA_DIR

$TIME $EXE 

