:: ******************* Compare Surrogates Run Script **************************
:: This script takes two sets of surrogates and compares them to one another.
:: The user sets an allowable difference tolerance and the program calculates
:: the differences between the two files for every entry in both surrogates files.
:: If the differences are greater than the allowable difference tolerance, the program
:: alerts the user, otherwise, the program gives a message of "Success!".
::
:: Script created by : C. Seppanen, MCNC-Environmental Modeling Center
:: Last edited : June 2005
:: 
::*********************************************************************


@echo off

:: Set installation directory
set BASDIR=..

set DEBUG_OUTPUT=Y

:: Set grid name
set GRID=M_08_99NASH

:: Set executable location
set EXE=%BASDIR%\diffsurr.exe

:: Set new and reference surrogates
set NEW_SRGS=%BASDIR%\output\merged_surrogates.txt
set REFERENCE_SRGS=%BASDIR%\data\merged_surrogates_ref.txt

set TOLERANCE=1E-5
set EFLAG=0

echo Comparing %NEW_SRGS%
echo to reference %REFERENCE_SRGS%

echo Testing category 7
set CATEGORY=7
%EXE% %NEW_SRGS% %CATEGORY% %REFERENCE_SRGS% %CATEGORY% %TOLERANCE%
if %errorlevel% NEQ 0 (
   echo ERROR comparing surrogates for category %CATEGORY% 
   set EFLAG=1
)

echo Testing category 8
set CATEGORY=8
%EXE% %NEW_SRGS% %CATEGORY% %REFERENCE_SRGS% %CATEGORY% %TOLERANCE%
if %errorlevel% NEQ 0 (
   echo ERROR comparing surrogates for category %CATEGORY% 
   set EFLAG=1
)

echo Testing category 300
set CATEGORY=300
%EXE% %NEW_SRGS% %CATEGORY% %REFERENCE_SRGS% %CATEGORY% %TOLERANCE%
if %errorlevel% NEQ 0 (
   echo ERROR comparing surrogates for category %CATEGORY% 
   set EFLAG=1
)

:error
if %eflag% == 1 (
   echo Test suite failed!
) else (
   echo Test suite successful!
)
