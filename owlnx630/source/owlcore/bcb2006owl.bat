@echo off

if not "%BCB2006ROOT%" == "" goto skip

set BCB2006ROOT=C:\Tools\Borland\BDS\4.0

:skip

set CB2010ROOT=
set CB2009ROOT=
set CB2007ROOT=
rem set BCB2006ROOT=
set BCB6ROOT=
set BCC55ROOT=
set BC5ROOT=

call bcmake.bat
