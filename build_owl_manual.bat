@echo off
echo Compiling essential OWL files manually for railcontrol...

cd C:\Apps\owlnx630\source\owlcore

set BC5ROOT=C:\Apps\BC5
set PATH=C:\Apps\BC5\bin;%PATH%

rem Create output directory
if not exist obj mkdir obj
if not exist ..\..\lib mkdir ..\..\lib

echo Compiling core OWL files...

rem Compile essential source files one by one
bcc32 -c -DOWL5_COMPAT -DMT -DOWLSECTION -DWIN32 -DCOMPVER=2 -DOWLVER=630 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\include -o obj\applicat.obj applicat.cpp
if errorlevel 1 goto error

bcc32 -c -DOWL5_COMPAT -DMT -DOWLSECTION -DWIN32 -DCOMPVER=2 -DOWLVER=630 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\include -o obj\framewin.obj framewin.cpp
if errorlevel 1 goto error

bcc32 -c -DOWL5_COMPAT -DMT -DOWLSECTION -DWIN32 -DCOMPVER=2 -DOWLVER=630 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\include -o obj\window.obj window.cpp
if errorlevel 1 goto error

echo Core compilation successful!
echo.
echo Object files created:
dir obj\*.obj

goto end

:error
echo Compilation failed!

:end
