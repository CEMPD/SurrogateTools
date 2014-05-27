::******************* Convert Shape From Envt Run Script *********************
:: This script converts a shape file from one map projection to another
::
:: In this version of the script, the map projection is obtained from
:: a pre-set environment variable
::
:: Note: the .proj component of the shape file is not created.
::
::****************************************************************************
@echo off
:: Location of executable
set BASDIR=..

set DEBUG_OUTPUT=Y

set MIMS_EXE=%BASDIR%\allocator.exe

set MIMS_PROCESSING=CONVERT_SHAPE

set OUTPUT_FILE_TYPE=RegularGrid
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_NAME=%1
set OUTPUT_FILE_NAME=%2

:: use INPUT_POLY_MAP_PRJN from preset environment variable
set INPUT_FILE_ELLIPSOID=SPHERE
set OUTPUT_FILE_MAP_PRJN=LATLON
set OUTPUT_FILE_ELLIPSOID=SPHERE

echo Converting from %INPUT_FILE_MAP_PRJN% to %OUTPUT_FILE_MAP_PRJN%
echo Input file = %INPUT_FILE_NAME
echo Output file = %OUTPUT_FILE_NAME
%MIMS_EXE%
copy %INPUT_FILE_NAME%.dbf %OUTPUT_FILE_NAME%.dbf
