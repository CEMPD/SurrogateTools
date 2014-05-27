::******************* Allocate Shapefiles Run Script **************************
:: Allocates a set of point observations (emissions)  data to a Regular Grid
:: March 2005, BB
:: Dec. 2007, LR -- projection specification changes
::*****************************************************************************


set DEBUG_OUTPUT=Y

:: Set executable
set MIMSDIR=..
set EXE="%MIMSDIR%\allocator.exe"

set TIME="time /t"
:: Set Input Directory
set DATADIR=%MIMSDIR%\data
set OUTPUT=%MIMSDIR%\output

:: Select method of spatial analysis

set MIMS_PROCESSING=ALLOCATE

::set "data" shapefile parameters
set GRIDDESC=%DATADIR%\GRIDDESC.txt

::set parameters for file being allocated
set INPUT_FILE_NAME=%DATADIR%\tn_fake_emis.csv
set INPUT_FILE_DELIMITER=COMMA
set INPUT_FILE_TYPE=PointFile
set INPUT_FILE_MAP_PRJN=+proj=latlong
set INPUT_FILE_XCOL=LONG
set INPUT_FILE_YCOL=LAT
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set INPUT_FILE_DELIM=COMMA
set ALLOCATE_ATTRS=NOX,VOC,PM_10
set ALLOC_MODE_FILE=ALL_AGGREGATE

:: Set name and path of resulting shapefile
set OUTPUT_FILE_NAME=%OUTPUT%\gridded_obs
set OUTPUT_FILE_TYPE=RegularGrid
set OUTPUT_GRID_NAME=M_08_99NASH
set OUTPUT_FILE_MAP_PRJN=M_08_99NASH
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

echo "Allocating observations to a grid"
%TIME% 
%EXE%
