::******************* Allocate Shapefiles Run Script **************************
:: Allocates a polygon to a grid, tests the discrete overlap function
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

if NOT EXIST %DATADIR%\discrete_modes.txt (
echo "Generating mode file"
echo "ATTRIBUTE=FIPS_CODE:DISCRETECENTROID" > %DATADIR%\discrete_modes.txt
echo "ATTRIBUTE=COUNTY:DISCRETEOVERLAP" >> %DATADIR%\discrete_modes.txt
)


:: Select method of spatial analysis

set MIMS_PROCESSING=ALLOCATE

set TIME="time /t"

::set "data" shapefile parameters
set GRIDDESC=%DATADIR%\GRIDDESC.txt

::set parameters for file being allocated
set INPUT_FILE_NAME=%DATADIR%\cnty_tn
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_MAP_PRJN=+proj=latlong
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set ALLOCATE_ATTRS=FIPS_CODE,COUNTY
set ALLOC_MODE_FILE=discrete_modes.txt

:: Set name and path of resulting shapefile
set OUTPUT_FILE_NAME=%OUTPUT%\gridded_county
set OUTPUT_FILE_TYPE=RegularGrid
set OUTPUT_GRID_NAME=M_08_99NASH
set OUTPUT_FILE_MAP_PRJN=M_08_99NASH
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

echo "Allocating discrete attributes of counties to a grid"
%TIME%
%EXE%
