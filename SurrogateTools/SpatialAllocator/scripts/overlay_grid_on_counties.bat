::******************* Overlay Run Script **************************************
::
::  Example of Overlay mode for MIMS Spatial Allocator
::  This example overlays counties with a grid
::  Created 3/30/2005 by Ben Brunk, Carolina Environmental Program
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

:: method of spatial analysis
set MIMS_PROCESSING=OVERLAY

set TIME=time /t

set OVERLAY_SHAPE=M_08_99NASH
set OVERLAY_TYPE=RegularGrid
set OVERLAY_MAP_PRJN=M_08_99NASH
set OVERLAY_ELLIPSOID=+a=6370997.0,+b=6370997.0
set OVERLAY_ATTRS=COUNTY
set INPUT_FILE_NAME=%DATADIR%\cnty_tn.shp
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set INPUT_FILE_MAP_PRJN=+proj=latlong
set GRIDDESC=%DATADIR%\GRIDDESC.txt
set OVERLAY_OUT_TYPE=Stdout
set OVERLAY_OUT_NAME=Stdout
set OVERLAY_OUT_DELIM=COMMA
set WRITE_HEADER=N


echo "Overlaying counties with a grid"
%TIME% 
%EXE%
