rem
rem ObjectWindows
rem
:@echo off

if not "%MSVCDir%" == "" goto build

if not "%VS6ROOT%" == "" goto skip

set VS6ROOT=C:\Program Files\Microsoft Visual Studio

:skip
call "%VS6ROOT%\VC98\bin\VCVARS32.BAT"

:build

: No Debug
echo Compile with MS VC 9.0 OCF6V.DLL STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsocfdll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 ALIGN=4 %1 %2 %3 %4 %5 %6 %7 %8 %9

: Debug Multithreaded
echo Compile with MS VC 9.0 OCF6DV.DLL STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsocfdll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 ALIGN=4 %1 %2 %3 %4 %5 %6 %7 %8 %9

echo Compile with MS VC 9.0 static OCF6V.LIB STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsocflib.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 ALIGN=4 %1 %2 %3 %4 %5 %6 %7 %8 %9

echo Compile with MS VC 9.0 static OCF6VD.LIB STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsocflib.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 ALIGN=4 %1 %2 %3 %4 %5 %6 %7 %8 %9

