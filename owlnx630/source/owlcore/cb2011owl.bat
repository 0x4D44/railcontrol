@echo off

if not "%CB2011ROOT%" == "" goto skip

set CB2011ROOT=C:\Progra~1\Embarcadero\RADStu~1\8.0

:skip

set CB2010ROOT=
set CB2009ROOT=
set CB2007ROOT=
set BCB2006ROOT=
set BCB6ROOT=
set BCC55ROOT=
set BC5ROOT=

call bcmake.bat
