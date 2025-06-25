@echo off
echo Building OWLNext 6.30 static library with all compatibility modes...

cd C:\Apps\owlnx630\source\owlcore

set BC5ROOT=C:\Apps\BC5
set PATH=C:\Apps\BC5\bin;%PATH%
set OWLVER=630

rem Create output directories
if not exist obj mkdir obj
if not exist ..\..\lib mkdir ..\..\lib

echo.
echo Compiling essential OWL source files with compatibility modes...
echo.

rem Core compilation flags matching railcontrol
set CFLAGS=-c -DOWL1_COMPAT -DOWL2_COMPAT -DOWL5_COMPAT -DBI_SUPPRESS_OLE -DMT -DOWLSECTION -DWIN32 -DCOMPVER=2 -DOWLVER=630 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\include

rem Compile essential OWL files one by one
echo Compiling applicat.cpp...
bcc32 %CFLAGS% applicat.cpp
if errorlevel 1 goto error
move applicat.obj obj\

echo Compiling framewin.cpp...
bcc32 %CFLAGS% framewin.cpp  
if errorlevel 1 goto error
move framewin.obj obj\

echo Compiling window.cpp...
bcc32 %CFLAGS% window.cpp
if errorlevel 1 goto error
move window.obj obj\

echo Compiling dialog.cpp...
bcc32 %CFLAGS% dialog.cpp
if errorlevel 1 goto error
move dialog.obj obj\

echo Compiling module.cpp...
bcc32 %CFLAGS% module.cpp
if errorlevel 1 goto error
move module.obj obj\

echo.
echo Creating static library...
rem Use tlib to create static library
tlib ..\..\lib\owl630bc5.lib +obj\applicat.obj +obj\framewin.obj +obj\window.obj +obj\dialog.obj +obj\module.obj

echo.
echo Build complete!
echo Library created: ..\..\lib\owl630bc5.lib
dir ..\..\lib\owl630bc5.lib

goto end

:error
echo.
echo Compilation failed on %errorlevel%!

:end
