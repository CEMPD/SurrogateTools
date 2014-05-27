::******************* Allocate Shapefiles Run Script **************************
:: Allocates a polygon shapefile's data to a Regular Grid
:: March 2005, BB
:: Dec. 2007, LR -- projection specification changes
::*****************************************************************************


set DEBUG_OUTPUT=Y

:: Set executable
set MIMSDIR=..
set EXE="%MIMSDIR%\allocator.exe"

:: Set Input Directory
set DATADIR=%MIMSDIR%\data
set OUTPUT=%MIMSDIR%\output

:: Select method of spatial analysis

set MIMS_PROCESSING=ALLOCATE

set TIME="time /t"
::set "data" shapefile parameters
set GRIDDESC=%DATADIR%\GRIDDESC.txt

::set parameters for file being allocated
set INPUT_FILE_NAME=%DATADIR%\tn_pophous
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set ALLOCATE_ATTRS=POP2000,HOUSEHOLDS
set ALLOC_MODE_FILE=ALL_AGGREGATE

:: Set name and path of resulting shapefile
set OUTPUT_FILE_NAME=%OUTPUT%\grid_pophous
set OUTPUT_FILE_TYPE=RegularGrid
set OUTPUT_GRID_NAME=M_08_99NASH
set OUTPUT_FILE_MAP_PRJN=M_08_99NASH
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

::echo "Allocating tracts to county for population and housing"
%TIME%
%EXE%
