@echo off

if not "%BCC55ROOT%" == "" goto skip

set BCC55ROOT=C:\Tools\Borland\BCC55

:skip


set CB2010ROOT=
set CB2009ROOT=
set CB2007ROOT=
set BCB2006ROOT=
set BCB6ROOT=
set BC5ROOT=

call bcmake.bat

