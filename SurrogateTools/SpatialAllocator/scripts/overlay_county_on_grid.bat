::******************* Overlay Run Script **************************************
::
::  Example of Overlay mode for MIMS Spatial Allocator
::  This example uses a shapefile overlaying a Regular Grid
::  Created 3/30/2005 by Ben Brunk, Carolina Environmental Program
::
::  Dec. 2007, LR -- projection specification changes
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

set TIME="time /t"

set OVERLAY_SHAPE=%DATADIR%\cnty_tn.shp
set OVERLAY_TYPE=ShapeFile
set OVERLAY_MAP_PRJN=+proj=latlong
set OVERLAY_ELLIPSOID=+a=6370997.0,+b=6370997.0
:: will print row and col
set OVERLAY_ATTRS=COL,ROW
set INPUT_FILE_NAME=M_08_99NASH
set INPUT_FILE_TYPE=RegularGrid
set INPUT_FILE_MAP_PRJN=M_08_99NASH
set GRIDDESC=%DATADIR%\GRIDDESC.txt
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set OVERLAY_OUT_TYPE=Stdout
set OVERLAY_OUT_NAME=Stdout
set OVERLAY_OUT_DELIM=COMMA
set WRITE_HEADER=Y


echo "Overlaying grid with county shapefile"
%TIME%
%EXE%
