@echo off
echo Building OWLNext with minimal options for BC 5.02...

cd C:\Apps\owlnx630\source\owlcore

set BC5ROOT=C:\Apps\BC5
set PATH=C:\Apps\BC5\bin;%PATH%

echo Building OWLNext static library with OWL5_COMPAT...

rem Set version manually (from version.h: 6,30,15,6654)
set OWLVER=630

make -f bc.mak -a -DOWL5_COMPAT -DMT -DOWLSECTION WIN32=1 COMPVER=2 OWLVER=%OWLVER% BCBROOT=C:\Apps\BC5

echo Build complete.
echo.
echo Checking for output files:
dir obj\*.obj
echo.
dir ..\..\lib\owl*.lib
