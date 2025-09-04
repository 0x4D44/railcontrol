@echo off

if not "%BC5ROOT%" == "" goto skip

set BC5ROOT=C:\Tools\Borland\BC5

:skip

set CB2010ROOT=
set CB2009ROOT=
set CB2007ROOT=
set BCB2006ROOT=
set BCB6ROOT=
set BCC55ROOT=
rem set BC5ROOT=

call bcmake.bat
