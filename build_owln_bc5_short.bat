@echo off
rem Build OWLNext 6.30 static (BC5) with ultra-short paths and clean env
rem Use cmd.exe (not PowerShell). Run from repo root.

setlocal enableextensions enabledelayedexpansion

rem Minimal Windows + BC5 environment (fixes MAKE invoking builtins like IF)
if not defined SystemRoot set SystemRoot=C:\Windows
set COMSPEC=%SystemRoot%\System32\cmd.exe
set WINDIR=%SystemRoot%
set PATH=C:\Apps\BC5\bin;%SystemRoot%\System32
if not exist C:\T mkdir C:\T >nul 2>&1
set TEMP=C:\T
set TMP=C:\T

rem Toolchain + OWLNext build switches
set BC5ROOT=C:\Apps\BC5
set COMPVER=2
set OWLVER=630
set MT=1
set USE_RSP=1

rem Map OWLNext root to a 1-letter drive for shortest commands
pushd %~dp0
if not exist owlnx630\source\owlcore\bc.mak (
  echo ERROR: owlnx630 sources not found relative to this script.
  exit /b 1
)
subst O: "%~dp0owlnx630" >nul 2>&1
set OWLBUILDROOT=O:\
set OBJROOT=O:\o
set TARGETLIBDIR=O:\lib
set TARGETDIR=O:\bin
if not exist O:\lib mkdir O:\lib >nul 2>&1
if not exist O:\bin mkdir O:\bin >nul 2>&1
if not exist O:\o   mkdir O:\o   >nul 2>&1

echo === Building OWLNext (static MT, non-Unicode) ===
cd /d O:\source\owlcore || exit /b 1
make -f bc.mak -a -c -l- || goto :make_error

echo.
echo === Selecting built library variant ===
set LAST=
for /f "delims=" %%F in ('dir /b /od O:\lib\owl-*.lib') do set LAST=%%F
if not defined LAST (
  echo ERROR: No owl-*.lib found in O:\lib
  goto :make_error
)
echo Found: !LAST!
if not exist "%~dp0owlnx630\lib" mkdir "%~dp0owlnx630\lib" >nul 2>&1
copy /y "O:\lib\!LAST!" "%~dp0owlnx630\lib\owl630bc5.lib" >nul || goto :copy_error

echo Done. Aliased to owlnx630\lib\owl630bc5.lib
echo You can now run build_local.bat
goto :eof

:make_error
echo *** MAKE failed. If you trimmed your environment, ensure COMSPEC and SystemRoot are set. ***
exit /b 2

:copy_error
echo *** Failed to copy library to alias path. ***
exit /b 3

