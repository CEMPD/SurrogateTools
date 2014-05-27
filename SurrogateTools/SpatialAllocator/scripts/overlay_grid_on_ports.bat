::******************* Overlay Run Script **************************************
::
::  Example of Overlay mode for MIMS Spatial Allocator
::  Example uses a Bounding Box (grid) overlaying a point shapefile
::  Created 3/30/2005 by Ben Brunk, Carolina Environmental Program
::
::*****************************************************************************

:: Set debug output
set DEBUG_OUTPUT=Y

:: Set executable
set MIMSDIR=..
set EXE="%MIMSDIR%\allocator.exe"

:: Set Input Directory
set DATADIR=%MIMSDIR%\data
set OUTPUT=%MIMSDIR%\output

:: Select method of spatial analysis

set MIMS_PROCESSING=OVERLAY

set TIME=time /t

set OVERLAY_SHAPE=1000000.0,-536000.0,1368000.0,-200000.0
set OVERLAY_TYPE=BoundingBox
set OVERLAY_MAP_PRJN=+proj=lcc,+lon_0=-100.0,+lat_1=30.0,+lat_2=60.0,+lat_0=40.0
set OVERLAY_ELLIPSOID=+a=6370997.00,+b=6370997.00
set OVERLAY_ATTRS=NAME,BERTHS,LAT,LONG
set INPUT_FILE_NAME=%DATADIR%\tn_ports
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_ELLIPSOID=+a=6370997.00,+b=6370997.00
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lon_0=-100.0,+lat_1=30.0,+lat_2=60.0,+lat_0=40.0
set OVERLAY_OUT_TYPE=DelimitedFile
set OVERLAY_OUT_NAME=%OUTPUT%\ports_over_grid.csv
set OVERLAY_OUT_DELIM=COMMA
set WRITE_HEADER=Y
::set MAX_INPUT_FILE_SHAPES=20


echo "Overlaying ports with a grid (bounding box of grid)"
%TIME% 
%EXE%
