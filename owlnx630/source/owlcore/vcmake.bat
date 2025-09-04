@echo off

echo.
echo ===============================================
echo  OWLNext Build Script for Microsoft Visual C++
echo ===============================================
echo.

rem If the compiler is configured then try to identify it.
rem Otherwise search for a compiler environment to set up.
rem Try the newest known compiler first.
rem If all fails, quit with a message.

echo ---
echo --- Identifying the compiler
echo ---
echo.

if not defined VCINSTALLDIR goto try10
if not exist "%VCINSTALLDIR%\bin\cl.exe" goto error

cl 2>&1 | find "Version 16"
if "%errorlevel%"=="0" set COMPVER=10 & goto build

cl 2>&1 | find "Version 15"
if "%errorlevel%"=="0" set COMPVER=9 & goto build

cl 2>&1 | find "Version 14"
if "%errorlevel%"=="0" set COMPVER=8 & goto build

cl 2>&1 | find "Version 13"
if "%errorlevel%"=="0" set COMPVER=7 & goto build

goto error

:try10

if not exist "%VS100COMNTOOLS%VSVARS32.BAT" goto try9
call "%VS100COMNTOOLS%VSVARS32.BAT"
set COMPVER=10
goto build

:try9

if not exist "%VS90COMNTOOLS%VSVARS32.BAT" goto try8
call "%VS90COMNTOOLS%VSVARS32.BAT"
set COMPVER=9
goto build

:try8

if not exist "%VS80COMNTOOLS%VSVARS32.BAT" goto try71
call "%VS80COMNTOOLS%VSVARS32.BAT"
set COMPVER=8
goto build

:try71

if not exist "%VS71COMNTOOLS%VSVARS32.BAT" goto error
call "%VS71COMNTOOLS%VSVARS32.BAT"
set COMPVER=7
goto build

:error

echo ---
echo --- Error: Unable to locate a supported compiler.
echo --- See the script source for supported versions.
echo ---
goto exit

:build

echo COMPVER=%COMPVER%
echo.
set MAKE=nmake -fvc.mak
set COMMON=COMPVER=%COMPVER% %1 %2 %3 %4 %5 %6 %7 %8 %9
set DEBUGS=DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN"

rem Non-Unicode variants

echo ---
echo --- Building the OWLNext Release DLL
echo ---
%MAKE% DLL=1 %COMMON% 
echo.

echo ---
echo --- Building the OWLNext Debug DLL
echo ---
%MAKE% DLL=1 %DEBUGS% %COMMON% 
echo.

echo ---
echo --- Building the OWLNext Static Release Library
echo ---
%MAKE% %COMMON%
echo.

echo ---
echo --- Building the OWLNext Static Debug Library
echo ---
%MAKE% %DEBUGS% %COMMON%
echo.

rem Unicode variants

echo ---
echo --- Building the OWLNext Unicode Release DLL
echo ---
%MAKE% UNICODE=1 DLL=1 %COMMON% 
echo.

echo ---
echo --- Building the OWLNext Unicode Debug DLL
echo ---
%MAKE% UNICODE=1 DLL=1 %DEBUGS% %COMMON% 
echo.

echo ---
echo --- Building the OWLNext Unicode Static Release Library
echo ---
%MAKE% UNICODE=1 %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Static Debug Library
echo ---
%MAKE% UNICODE=1 %DEBUGS% %COMMON%
echo.

rem Report outputs

echo ---
echo --- Listing the target outputs after successful build
echo ---
echo.
dir ..\..\lib\*.lib ..\..\bin\*.dll

:exit

rem Clear local symbols.

set COMPVER=
set MAKE=
set COMMON=
set DEBUGS=
