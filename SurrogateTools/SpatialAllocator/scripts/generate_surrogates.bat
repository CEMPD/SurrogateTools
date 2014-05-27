::******************* Generate Surrogates Run Script **************************
:: This script generates surrogates for the MIMS spatial tool test case
:: (8km over Tennessee).
:: 
:: Script created by : Alison Eyth, Carolina Environmental Program
:: Last edited : June 2005
:: 
:: Dec. 2007, LR -- projection specification changes
::*********************************************************************


@echo off

:: Set installation directory

set BASDIR=..

set DEBUG_OUTPUT=N

:: Set directory for output surrogate and shape files

set WORK_DIR=%BASDIR%\output

:: Set Location of shapefiles
set DATA=%BASDIR%\data

mkdir %WORK_DIR%

:: Set Grid settings
set GRIDDESC=%DATA%\GRIDDESC.txt
set OUTPUT_GRID_NAME=M_08_99NASH
:: Set the ellipsoid to this value so that the surrogates will compare
:: to the reference set generated using a GIS.  This value should not be
:: used for any other applications.
set OUTPUT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

:: Set Location of executable
set MIMSDIR=%BASDIR%
set EXE=srgcreate.exe

:: Set name and path to temporary surrogate file
set SURROGATE_FILE=%WORK_DIR%\tmp_srg.%OUTPUT_GRID_NAME%.txt

:: Set name and path to final merged surrogate file from spatial tool  
set SRG_FILE=%WORK_DIR%\srg_%OUTPUT_GRID_NAME%.txt

:: WRITE_QASUM=YES prints surrogate sums by county in file
:: WRITE_SRG_NUMERATOR=YES writes surrogate numerator as comment in file
:: WRITE_SRG_DENOMINATOR=YES writes denminator (county totals) for srg weight
set WRITE_QASUM=YES
set WRITE_SRG_NUMERATOR=YES
set WRITE_SRG_DENOMINATOR=YES
set DENOMINATOR_THRESHOLD=1e-8

:: Print header info
set WRITE_HEADER=NO

:: Specify type of data files to use
set OUTPUT_FILE_TYPE=RegularGrid
set DATA_FILE_NAME_TYPE=ShapeFile
set WEIGHT_FILE_TYPE=ShapeFile

:: The data polygons should be the shape file containing county polygons
set DATA_FILE_NAME=%DATA%\cnty_tn
set DATA_ID_ATTR=FIPS_CODE
set DATA_FILE_MAP_PRJN=+proj=latlong
::set DATA_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0

:: An example of using a non-default named ellipsoid:
::set DATA_FILE_ELLIPSOID=+ellps=clrk66

:: Set the ellipsoid to this value so that the surrogates will compare
:: to the reference set generated using a GIS.  This value should not be
:: used for any other applications.
set DATA_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0


:: Set weight projection to that of the EPA files
set WEIGHT_FILE_MAP_PRJN=+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97
::set WEIGHT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0
set WEIGHT_FILE_ELLIPSOID=+a=6370997.0,+b=6370997.0


:: Set the WEIGHT_FUNCTION and FILTER_FILE to NONE in case they were set
:: from earlier executions of scripts
set WEIGHT_FUNCTION=NONE
set FILTER_FILE=NONE

set TIME="time /t"

set waserror=0

:: Generate surrogate header line
echo Writing header
%MIMSDIR%\%EXE% -header > %SRG_FILE%
if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)

:: Generate surrogates for each category
:: For each one, specify:
:: SURROGATE_ID (integer ID for type of surrogate)
:: WEIGHT_FILE_NAME (shapefile for weights), 
:: WEIGHT_ATTR_LIST (attribute to weight by (or NONE for area/length/count)
:: OUTPUT_FILE_NAME (optional - name of shapefile of gridded weights to output)


echo Generating airport surrogate
set SURROGATE_ID=2
set WEIGHT_FILE_NAME=%DATA%\us_air-pt
set WEIGHT_ATTR_LIST=NONE
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_airpt_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%


echo Generating land area surrogate
set SURROGATE_ID=3
set WEIGHT_FILE_NAME=NONE
set WEIGHT_ATTR_LIST=NONE
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_area_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%

echo Generating ports surrogate
set SURROGATE_ID=4
set WEIGHT_FILE_NAME=%DATA%\tn_ports
set WEIGHT_ATTR_LIST=BERTHS
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_ports_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%

echo Generating navigable H20 surrogate
set SURROGATE_ID=5
set WEIGHT_FILE_NAME=%DATA%\us_nav_h20
set WEIGHT_ATTR_LIST=LENGTH
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_navig_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%

echo Generating Highways surrogate
set SURROGATE_ID=6
set WEIGHT_FILE_NAME=%DATA%\tn_roads
set WEIGHT_ATTR_LIST=NONE
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_highway_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%

echo Generating population and housing surrogate
set SURROGATE_ID=7,8
set WEIGHT_FILE_NAME=%DATA%\tn_pophous
set WEIGHT_ATTR_LIST=HOUSEHOLDS,POP2000
set OUTPUT_FILE_NAME=%WORK_DIR%\grid_pop_%OUTPUT_GRID_NAME%
%TIME%
%MIMSDIR%\%EXE%

if %errorlevel% NEQ 0 (
   set waserror=1
   goto error
)
more %SURROGATE_FILE% >> %SRG_FILE%
del %SURROGATE_FILE%

:error
if %waserror% == 1 (
   echo ERROR GENERATING SURROGATES
)
