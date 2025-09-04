@echo off
REM =====================================================================
REM   RailControl - Streamlined Build Script
REM   Single command build for the OWLNext 6.30 railway simulation
REM =====================================================================

setlocal enabledelayedexpansion

echo ================================================================
echo   RailControl - OWLNext 6.30 Build System
echo   Building complete railway simulation application...
echo ================================================================
echo.

REM Setup compiler path
set path=c:\apps\bc5\bin;%path%

REM Clean previous build artifacts
echo Cleaning previous build...
if exist *.obj del /q *.obj >nul 2>&1
if exist railc.exe del /q railc.exe >nul 2>&1
if exist railc.map del /q railc.map >nul 2>&1

echo.
echo [1/3] Compiling source files...
echo ----------------------------------------

REM Compile all source files in batches for better efficiency
REM Core application files
echo   Core modules...
bcc32 +BccW32_simple.cfg -c railc.cpp about.cpp startup.cpp start.cpp
if errorlevel 1 goto compilation_error

echo   UI components...
bcc32 +BccW32_simple.cfg -c toolbar.cpp toolbutt.cpp statbar.cpp platform.cpp
if errorlevel 1 goto compilation_error

echo   Railway logic...
bcc32 +BccW32_simple.cfg -c layout.cpp arrivals.cpp departur.cpp locoyard.cpp
if errorlevel 1 goto compilation_error

echo   Data management...
bcc32 +BccW32_simple.cfg -c section.cpp platdata.cpp ovlpdata.cpp routes.cpp locos.cpp timetabl.cpp
if errorlevel 1 goto compilation_error

echo   Dialog and configuration...
bcc32 +BccW32_simple.cfg -c configur.cpp selector.cpp finish.cpp
if errorlevel 1 goto compilation_error

echo.
echo [2/3] Building resources...
echo ----------------------------------------
if not exist resource mkdir resource >nul 2>&1
brc32 -IC:\Apps\BC5\INCLUDE -w32 -R -FO resource\railc.res resource\railc.rc
if errorlevel 1 goto resource_error

echo.
echo [3/3] Linking executable...
echo ----------------------------------------
tlink32 @link_working.rsp
REM Note: Linker may show warnings but still produces working executable

echo.
echo ================================================================
if exist railc.exe (
    echo   *** BUILD COMPLETED SUCCESSFULLY ***
    echo.
    for %%F in (railc.exe) do (
        echo   Executable: railc.exe [%%~zF bytes]
    )
    echo   Framework:  OWLNext 6.30 with compatibility modes
    echo   Compiler:   Borland C++ 5.02
    echo   Features:   Complete railway simulation system
    echo.
    echo   Ready to run: railc.exe
    echo   Help system:  F1 or Help menu
    echo   Layouts:      FAST, KINGSX, QUEENST, WAVERLY
    echo.
) else (
    echo   *** BUILD COMPLETED WITH ISSUES ***
    echo   Executable was not created - check error messages above
    goto build_failed
)

echo ================================================================
goto build_success

:compilation_error
echo.
echo *** COMPILATION ERROR ***
echo One or more source files failed to compile.
echo Check the error messages above for details.
goto build_failed

:resource_error
echo.
echo *** RESOURCE BUILD ERROR ***
echo Failed to build Windows resources.
echo Verify resource files exist in the resource directory.
goto build_failed

:build_failed
echo Build failed. Please review error messages above.
exit /b 1

:build_success
echo Build completed successfully!
if "%1" neq "nopause" pause
exit /b 0
