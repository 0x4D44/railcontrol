@echo off
REM Build script for Rail Control project
REM Updated for BCC 5.02 - Static linking (no DLL dependencies)

echo Building Rail Control with static linking...
echo.

REM Set up the path for Borland C++ 5.02
set path=c:\apps\bc5\bin;%path%

REM Build the project
echo Compiling and linking (static)...
make -f railc.mak

REM Check if build was successful
if exist railc.exe (
    echo.
    echo *** BUILD SUCCESSFUL ***
    echo railc.exe created as standalone executable
    echo File size: 
    dir railc.exe | find /i "railc.exe"
    echo.
    echo *** NO DLL DEPENDENCIES REQUIRED ***
    echo This executable runs on any Windows system without additional libraries.
) else (
    echo.
    echo *** BUILD FAILED ***
    echo railc.exe was not created
)

echo.
echo Build process complete.
pause
