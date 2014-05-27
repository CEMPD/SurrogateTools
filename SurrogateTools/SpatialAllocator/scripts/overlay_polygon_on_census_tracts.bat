::******************* Overlay Run Script **************************************
::
::  Example of Overlay mode for MIMS Spatial Allocator
::  This example overlays census tracts with a polygon file containing
::  the coordinates for the bounding box of the M_08_99NASH grid
::  Note:  A polygon file is an ASCII csv file and is not the same as
::  a polygon shapefile, which is a binary file
::  Created 4/18/2005 by Ben Brunk, Carolina Environmental Program
::
::  Dec. 2007, LR -- projection specification changes
::*****************************************************************************

:: Set debug output
set DEBUG_OUTPUT=N

:: Set executable
set MIMSDIR=..
set EXE="%MIMSDIR%\allocator.exe"

:: Set Input Directory
set DATADIR=%MIMSDIR%\data
set OUTPUT=%MIMSDIR%\output

:: Select method of spatial analysis

set MIMS_PROCESSING=OVERLAY

set TIME=time /t

set OVERLAY_SHAPE=%DATADIR%\polygonfile.csv
set OVERLAY_TYPE=PolygonFile
set OVERLAY_MAP_PRJN=+proj=lcc,+lon_0=-100.0,+lat_1=30.0,+lat_2=60.0,+lat_0=40.0
set OVERLAY_DELIM=COMMA
set OVERLAY_ELLIPSOID=+a=6370997.0,+b=6370997.0
set OVERLAY_ATTRS=FIPSSTCO,POP2000,HOUSEHOLDS
set INPUT_FILE_NAME=%DATADIR%\tn_pophous
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lon_0=-100.0,+lat_1=30.0,+lat_2=60.0,+lat_0=40.0
set INPUT_FILE_DELIM=COMMA
set GRIDDESC=%DATADIR%\GRIDDESC.txt
set OVERLAY_OUT_TYPE=DelimitedFile
set OVERLAY_OUT_NAME=%OUTPUT%\overlay_census_tracts_over_polygonfile.csv
set OVERLAY_OUT_DELIM=COMMA
set WRITE_HEADER=Y


echo "Overlaying census tracts with a polygon"
%TIME%
%EXE%
