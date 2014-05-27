::******************* Allocate Shapefiles Run Script **************************
:: Allocates point source data into a polygon
:: March 2005, BB
:: Dec. 2007, LR -- projection specification changes
::*****************************************************************************

set DEBUG_OUTPUT=Y

:: Set executable
set MIMSDIR=..
set EXE=%MIMSDIR%\allocator.exe

:: Set Input Directory
set DATADIR=%MIMSDIR%\data
set OUTPUT=%MIMSDIR%\output

set TIME=time /t

:: Select method of spatial analysis
set MIMS_PROCESSING=ALLOCATE

:: Set name and path of shapefile being allocated to
set OUTPUT_POLY_FILE=%DATADIR%\cnty_tn
set OUTPUT_POLY_MAP_PRJN=+proj=latlong
set OUTPUT_POLY_ELLIPSOID=+a=6370997.0,+b=6370997.0
set OUTPUT_POLY_ATTRS=FIPS_CODE,COUNTY
set OUTPUT_POLY_TYPE=ShapeFile

:: Set Shapefile from which to allocate data
set INPUT_FILE_NAME=%DATADIR%\tn_ports
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_XCOL=LONG
set INPUT_FILE_YCOL=LAT
set INPUT_FILE_DELIM=COMMA
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set ALLOCATE_ATTRS=BERTHS
set ALLOC_MODE_FILE=ALL_AGGREGATE

:: Set name and path of resulting shapefile
set OUTPUT_FILE_NAME=%OUTPUT%\agg_berths
set OUTPUT_FILE_TYPE=ShapeFile
set OUTPUT_FILE_MAP_PRJN=+proj=latlong
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

echo "Allocating ports to counties"
%TIME%
%EXE%
