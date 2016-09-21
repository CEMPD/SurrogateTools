#!/bin/csh -f
#******************* Filter Shapefile Run Script ******************************
#  This script demonstrates the use of a filter file in the MIMS
# Spatial Allocator
#
# Set FILTER_FILE to the name of the filter file
# Set DATA_FILE_NAME to the name of the input Shapefile
# Set OUTPUT_FILE_NAME to the name of the output Shapefile
#
# Script created by : Ben Brunk, Carolina Environmental Program
# Last edited : June 2005
# Modified By:  L.R., Nov. 2008
#
#****************************************************************************

setenv DEBUG_OUTPUT Y

# Location of executable
setenv EXE $SA_HOME/bin/32bits/allocator.exe

setenv MIMS_PROCESSING FILTER_SHAPE

# set FILTER_FILE to the name of the filter file to use
setenv FILTER_FILE $SA_HOME/data/state_filter.txt

# set INPUT_FILE_NAME to the name of the input Shapefile
setenv INPUT_FILE_NAME ../data/surfzone_poly_st
setenv INPUT_FILE_TYPE ShapeFile

# set OUTPUT_FILE_NAME to the name of the output Shapefile
setenv OUTPUT_FILE_NAME ../output/surfzone_NC_SC

echo "Input Shapefile = $INPUT_FILE_NAME"
echo "Output Shapefile = $OUTPUT_FILE_NAME"
echo "Filter file = $FILTER_FILE"

# Housekeeping 
rm -f $OUTPUT_FILE_NAME.*
$EXE
