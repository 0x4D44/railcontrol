@echo off

if not "%CB2009ROOT%" == "" goto skip

set CB2009ROOT=C:\Progra~1\CodeGear\RADStu~1\6.0

:skip

set CB2010ROOT=
rem set CB2009ROOT=
set CB2007ROOT=
set BCB2006ROOT=
set BCB6ROOT=
set BCC55ROOT=
set BC5ROOT=

call bcmake.bat

