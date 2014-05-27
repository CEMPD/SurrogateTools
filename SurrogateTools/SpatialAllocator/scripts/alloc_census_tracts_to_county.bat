::******************* Allocate Shapefiles Run Script **************************
:: Example of how to allocate data from one polygon shapefile to another
:: polygon shapefile.
::
:: May 2005, BDB
:: Dec. 2007, LR
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
:: Set name and path of shapefile having data allocated to it
set OUTPUT_POLY_FILE=%DATADIR%\cnty_tn
set OUTPUT_POLY_MAP_PRJN=+proj=latlong
set OUTPUT_POLY_ELLIPSOID=+a=6370997.0,+b=6370997.0
::set OUTPUT_POLY_ATTRS=FIPS_CODE,COUNTY
set OUTPUT_POLY_ATTRS=COUNTY,FIPS_CODE
set OUTPUT_POLY_TYPE=ShapeFile

:: Set Shapefile from which to allocate data
set INPUT_FILE_NAME=%DATADIR%\tn_pophous
set INPUT_FILE_TYPE=ShapeFile
set INPUT_FILE_MAP_PRJN=+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97
set INPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set ALLOCATE_ATTRS=POP2000,HOUSEHOLDS,HDENS
set ALLOC_MODE_FILE=atts_pophous.txt

:: Set name and path of resulting shapefile
set OUTPUT_FILE_NAME=%OUTPUT%\county_pophous
set OUTPUT_FILE_TYPE=ShapeFile
set OUTPUT_FILE_MAP_PRJN=+proj=latlong
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0


echo "Allocating census population tracts to counties"
%TIME% 
%EXE%
