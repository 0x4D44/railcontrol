@echo off
REM ================================================================
REM   RailControl - Local OWLNext (6.30) Build (Borland C++ 5.02)
REM   Uses local OWLNext sources under .\owlnx630 instead of C:\Apps\owlnx630
REM ================================================================

setlocal enabledelayedexpansion

echo ================================================================
echo   RailControl - Local OWLNext 6.30 Build
echo   Compiler: Borland C++ 5.02  (expects C:\Apps\BC5)
echo ================================================================
echo.

REM Ensure toolchain in PATH
set PATH=C:\Apps\BC5\bin;%PATH%

REM Check local OWLNext lib alias exists
if not exist owlnx630\lib (
  echo ERROR: Missing folder .\owlnx630\lib
  echo Hint: Build OWLNext first (see instructions below).
  goto how_to_build_owlnext
)

if not exist owlnx630\lib\owl630bc5.lib (
  echo ERROR: Missing .\owlnx630\lib\owl630bc5.lib
  echo This is an alias expected by RailControl's linker response.
  echo.
  goto how_to_build_owlnext
)

echo Cleaning outputs...
del /q *.obj 2>nul
del /q railc.exe railc.map 2>nul

echo.
echo [1/3] Compiling (local OWLNext includes)...
bcc32 +BccW32_local.cfg -c railc.cpp about.cpp startup.cpp start.cpp || goto compilation_error
bcc32 +BccW32_local.cfg -c toolbar.cpp toolbutt.cpp statbar.cpp platform.cpp || goto compilation_error
bcc32 +BccW32_local.cfg -c layout.cpp arrivals.cpp departur.cpp locoyard.cpp || goto compilation_error
bcc32 +BccW32_local.cfg -c section.cpp platdata.cpp ovlpdata.cpp routes.cpp locos.cpp timetabl.cpp || goto compilation_error
bcc32 +BccW32_local.cfg -c configur.cpp selector.cpp finish.cpp || goto compilation_error

echo.
echo [2/3] Building resources...
brc32 -IC:\Apps\BC5\INCLUDE -w32 -R -FO resource\railc.res resource\railc.rc || goto resource_error

echo.
echo [3/3] Linking (local OWLNext libs)...
tlink32 @link_local.rsp
if not exist railc.exe goto link_error

echo.
echo *** BUILD COMPLETED SUCCESSFULLY ***
for %%F in (railc.exe) do echo   Executable: railc.exe [%%~zF bytes]
goto done

:compilation_error
echo *** COMPILATION ERROR ***
exit /b 1

:resource_error
echo *** RESOURCE BUILD ERROR ***
exit /b 1

:link_error
echo *** LINK ERROR ***
exit /b 1

:how_to_build_owlnext
echo To build OWLNext 6.30 with BC5 locally:
echo   1) Ensure Borland C++ 5.02 is installed at C:\Apps\BC5
echo   2) Open a CMD and run:
echo      set BC5ROOT=C:\Apps\BC5
echo      cd /d %~dp0owlnx630\source\owlcore
echo      bcmake.bat
echo   3) After build, copy the desired OWLNext static release lib to:
echo      .\owlnx630\lib\owl630bc5.lib
echo      (pick non-Unicode, Release, multithreaded static variant)
echo Then re-run this script.
exit /b 2

:done
endlocal
exit /b 0

