::******************* Convert Input Shape Run Script *************************
:: This script converts shape file from one map projection to another
::
:: The default map projection in this script is set to that of the input
:: data files in the data directory.
::
:: Note: the .proj component of the shape file is not created.
:: You must provide the shape file input and output names on the command
:: line.
::
::****************************************************************************
^M
:: Set debug output^M
set DEBUG_OUTPUT=Y^M
^M
@echo off
:: Location of executable
set MIMS_EXE=..\allocator.exe

set MIMS_PROCESSING=CONVERT_SHAPE

set OUTPUT_FILE_TYPE=RegularGrid
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_NAME=%1
set OUTPUT_FILE_NAME=%2

::set input projection for nashville grid to convert outputs to ll
::set INPUT_FILE_MAP_PRJN=+proj=lcc,+lat_1=30,+lat_2=60,+lat_0=40,+lon_0=-100
::set input projection to EPA Lambert to convert surrogate input files to ll
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97
set INPUT_FILE_ELLIPSOID=+a=6370997.00,+b=6370997.00^M
set OUTPUT_FILE_MAP_PRJN=+proj=latlong
set OUTPUT_FILE_ELLIPSOID=+a=6370997.00,+b=6370997.00^M

echo Usage: convert_shape input_shape_file output_shape_file
echo Converting from %INPUT_FILE_MAP_PRJN% to %OUTPUT_FILE_MAP_PRJN%
echo Input file = %INPUT_FILE_NAME%
echo Output file = %OUTPUT_FILE_NAME%
%MIMS_EXE%
copy %INPUT_FILE_NAME%.dbf %OUTPUT_FILE_NAME%.dbf
