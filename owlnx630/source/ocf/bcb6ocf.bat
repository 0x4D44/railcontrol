@echo off

if not "%BCB6ROOT%" == "" goto skip

set BCB6ROOT=C:\Tools\Borland\CBuilder6

:skip


set CB2010ROOT=
set CB2009ROOT=
set CB2007ROOT=
set BCB2006ROOT=
set BCC55ROOT=
set BC5ROOT=

call bcmake.bat
