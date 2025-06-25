@echo off
echo Testing simple OWL compilation...

cd C:\Apps\owlnx630\source\owlcore

set BC5ROOT=C:\Apps\BC5
set PATH=C:\Apps\BC5\bin;%PATH%

echo Compiling applicat.cpp...

bcc32 -c -DOWL5_COMPAT -DMT -DOWLSECTION -DWIN32 -DCOMPVER=2 -DOWLVER=630 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\include applicat.cpp

echo Compilation result:
if exist applicat.obj echo applicat.obj created successfully
if not exist applicat.obj echo applicat.obj NOT created

dir *.obj
