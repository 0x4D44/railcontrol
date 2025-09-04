@echo off
setlocal enableextensions

: This script parses the OWLNext version number from the sources.
: It assumes the source file contains a single definition of the version number 
: formatted as follows:
: 
:   define _OWL_VERSION(v) v(Major,Minor,Revision,Build)
: 
: In the for-body (echo command) the following variables are defined:
: 
:   %%i = Major
:   %%j = Minor
:   %%k = Revision
:   %%l = Build
:
: The parsed version number is assigned to the OWLVER environment variable and written 
: to a temporary file "%TEMP%\owlver.tmp" with the following contents:
:
:   OWLVER=%OWLVER%
:
: The temporary file can be used by makefiles to include the OWLNext version number.

for %%i in (%systemroot%\system32\find.exe) do (set FIND=%%~si)

for /F "skip=2 tokens=5-8 delims=(,) " %%i in ('%FIND% "define _OWL_VERSION(v)" ..\..\include\owl\version.h') do set OWLVER=%%i%%j
echo OWLVER=%OWLVER% > %TEMP%\owlver.tmp
