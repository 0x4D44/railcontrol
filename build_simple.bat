@echo off
REM Simple build script for Rail Control
REM Avoids "Command arguments too long" error

echo Building Rail Control with simplified approach...
set path=c:\apps\bc5\bin;%path%

REM Clean previous build
if exist *.obj del *.obj
if exist railc.exe del railc.exe

echo Compiling source files...

REM Compile each source file individually with simple config
bcc32 +BccW32_simple.cfg -c railc.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c about.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c arrivals.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c departur.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c finish.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c configur.cpp  
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c layout.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c locos.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c locoyard.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c ovlpdata.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c platdata.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c platform.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c routes.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c section.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c selector.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c start.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c startup.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c statbar.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c timetabl.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c toolbar.cpp
if errorlevel 1 goto error

bcc32 +BccW32_simple.cfg -c toolbutt.cpp
if errorlevel 1 goto error

echo Building resources...
if not exist resource mkdir resource
brc32 -IC:\Apps\BC5\INCLUDE -w32 -R -FO resource\railc.res resource\railc.rc
if errorlevel 1 goto error

echo Linking...
tlink32 @link_fixed.rsp
if errorlevel 1 goto error

echo.
echo *** BUILD SUCCESSFUL ***
goto end

:error
echo.
echo *** BUILD FAILED ***
echo Error occurred during compilation

:end
pause
