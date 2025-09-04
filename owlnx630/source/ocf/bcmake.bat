@echo off

echo.
echo =====================================================
echo  OWLNext Build Script for Borland C++ and C++Builder
echo =====================================================
echo.

rem If the compiler is configured then try to identify it.
rem Try the newest known compiler first.
rem Otherwise report an error.

echo ---
echo --- Identifying the compiler
echo ---
echo.

if not defined CB2011ROOT goto try11
set COMPROOT=%CB2011ROOT%
set COMPVER=12
goto parse_version

:try11
if not defined CB2010ROOT goto try10
set COMPROOT=%CB2010ROOT%
set COMPVER=11
goto parse_version

:try10
if not defined CB2009ROOT goto try9
set COMPROOT=%CB2009ROOT%
set COMPVER=10
goto parse_version

:try9
if not defined CB2007ROOT goto try8
set COMPROOT=%CB2007ROOT%
set COMPVER=9
goto parse_version

:try8
if not defined BCB2006ROOT goto try6
set COMPROOT=%BCB2006ROOT%
set COMPVER=8
goto parse_version

:try6
if not defined BCB6ROOT goto try5
set COMPROOT=%BCB6ROOT%
set COMPVER=6
goto parse_version

:try5
if not defined BCC55ROOT goto try2
set COMPROOT=%BCC55ROOT%
set COMPVER=5
goto parse_version

:try2
if not defined BC5ROOT goto error
set COMPROOT=%BC5ROOT%
set COMPVER=2
goto parse_version

:error

echo ---
echo --- Error: Unable to locate a supported compiler.
echo --- See the script source for supported versions.
echo ---
goto exit

:parse_version

echo COMPVER=%COMPVER%
echo.

echo ---
echo --- Parsing the version number from sources
echo ---
echo.

call parse_version.bat
echo OWLVER=%OWLVER%
echo.
if "%OWLVER%" == "" goto parse_error
goto build

:parse_error

echo ---
echo --- Error: Unable to parse the version number.
echo ---
goto exit

:build

set MAKE=%COMPROOT%\bin\make -f bc.mak -a -c -l+ 
set COMMON=-DNOCOMPILE_ASM -DNO_OWL5_COMPAT WIN32=1 COMPVER=%COMPVER% OWLVER=%OWLVER% BCBROOT=%COMPROOT% %1 %2 %3 %4 %5 %6 %7 %8 %9
set DEBUGS=DIAGS=2 DEBUG=2 CODEGUARD=2 

rem Multithreaded non-Unicode variants

echo ---
echo --- Building the OWLNext Static Release Library (multithreaded)
echo ---
%MAKE% -DMT -DOWLSECTION %COMMON%
echo.

echo ---
echo --- Building the OWLNext Release DLL (multithreaded)
echo ---
%MAKE% -DMT DLL=1 %COMMON%
echo.

echo ---
echo --- Building the OWLNext Static Debug Library (multithreaded)
echo ---
%MAKE% -DMT -DOWLSECTION %DEBUGS% %COMMON%
echo.

echo ---
echo --- Building the OWLNext Debug DLL (multithreaded)
echo ---
%MAKE% -DMT DLL=1 %DEBUGS% %COMMON%
echo.

rem Singlethreaded non-Unicode variants

echo ---
echo --- Building the OWLNext Static Release Library (singlethreaded)
echo ---
%MAKE% -DOWLSECTION %COMMON%
echo.

echo ---
echo --- Building the OWLNext Release DLL (singlethreaded)
echo ---
%MAKE% DLL=1 %COMMON%
echo.

echo ---
echo --- Building the OWLNext Static Debug Library (singlethreaded)
echo ---
%MAKE% -DOWLSECTION %DEBUGS% %COMMON%
echo.

echo ---
echo --- Building the OWLNext Debug DLL (singlethreaded)
echo ---
%MAKE% DLL=1 %DEBUGS% %COMMON%
echo.

rem Multithreaded Unicode variants

echo ---
echo --- Building the OWLNext Unicode Static Release Library (multithreaded)
echo ---
%MAKE% -DUNICODE -DMT -DOWLSECTION %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Release DLL (multithreaded)
echo ---
%MAKE% -DUNICODE -DMT DLL=1 %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Static Debug Library (multithreaded)
echo ---
%MAKE% -DUNICODE -DMT -DOWLSECTION %DEBUGS% %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Debug DLL (multithreaded)
echo ---
%MAKE% -DUNICODE -DMT DLL=1 %DEBUGS% %COMMON%
echo.

rem Singlethreaded Unicode variants

echo ---
echo --- Building the OWLNext Unicode Static Release Library (singlethreaded)
echo ---
%MAKE% -DUNICODE -DOWLSECTION %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Release DLL (singlethreaded)
echo ---
%MAKE% -DUNICODE DLL=1 %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Static Debug Library (singlethreaded)
echo ---
%MAKE% -DUNICODE -DOWLSECTION %DEBUGS% %COMMON%
echo.

echo ---
echo --- Building the OWLNext Unicode Debug DLL (singlethreaded)
echo ---
%MAKE% -DUNICODE DLL=1 %DEBUGS% %COMMON%
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