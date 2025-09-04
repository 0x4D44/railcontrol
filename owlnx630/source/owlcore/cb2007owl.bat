@echo off

if not "%CB2007ROOT%" == "" goto skip

set CB2007ROOT=C:\Progra~1\CodeGear\RADStu~1\5.0

:skip

set CB2010ROOT=
set CB2009ROOT=
rem set CB2007ROOT=
set BCB2006ROOT=
set BCB6ROOT=
set BCC55ROOT=
set BC5ROOT=

call bcmake.bat

