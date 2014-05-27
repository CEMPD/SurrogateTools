#!/bin/csh -f
#******************* Convert Shape Run Script **************************
# This script converts a shape file from one map projection to another
#
# The default map projection used in this file is that of the output
# grid used for the example.
#
# Note: the .proj component of the shape file is not created.
# You must provide the shape file input and output names on the command
# line.
#
#****************************************************************************

setenv DEBUG_OUTPUT N

# Location of executable
setenv MIMS_EXE ../allocator.exe

if ($#argv < 2) then
  echo "Usage: convert_shape.csh input_shapefile output_shapefile (no file extensions)"
  exit 2
endif

setenv MIMS_PROCESSING CONVERT_SHAPE

setenv OUTPUT_FILE_TYPE    RegularGrid	   # Type of grid
setenv INPUT_FILE_TYPE   ShapeFile	   # Type of input data file
setenv INPUT_FILE_NAME    $argv[1]       # shape file name - no extension
setenv OUTPUT_FILE_NAME   $argv[2]    # shape file name - no extension

#set input projection for nashville grid to convert outputs to ll
setenv INPUT_FILE_MAP_PRJN "+proj=lcc,+lat_1=30,+lat_2=60,+lat_0=40,+lon_0=-100"

#set input projection to EPA Lambert to convert surrogate input files to ll
#setenv INPUT_FILE_MAP_PRJN "+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97"
setenv INPUT_FILE_ELLIPSOID  SPHERE
setenv OUTPUT_FILE_MAP_PRJN   LATLON       # map projection for data poly file
setenv OUTPUT_FILE_ELLIPSOID  SPHERE

# ssusuage is good on the SGI for mem & CPU usage
#setenv TIME ssusage
setenv TIME time

echo "Converting from $INPUT_FILE_MAP_PRJN to $OUTPUT_FILE_MAP_PRJN"
echo "Input file = $INPUT_FILE_NAME"
echo "Output file = $OUTPUT_FILE_NAME"
$TIME $MIMS_EXE
