rem
rem ObjectWindows
rem
:@echo off

: For MS VC 6.0

if not "%MSVCDir%" == "" goto build

if not "%VS6ROOT%" == "" goto skip

set VS6ROOT=C:\Program Files\Microsoft Visual Studio

:skip
call "%VS6ROOT%\VC98\bin\VCVARS32.BAT"

:build

echo Compile with MS VC 6.0 OWL6V.DLL STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsowldll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9

echo Compile with MS VC 6.0 OWL6DV.DLL STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmsowldll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9

:------------------------------------------------------------------------------------------
:echo Compile with MS VC 6.0 OWL6VU.DLL STD RTL + UNICODE
:%VS6ROOT%\VC98\bin\nmake -fmsowldll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 UNICODE=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9

:echo Compile with MS VC 6.0 OWL6DVU.DLL STD RTL + UNICODE
:%VS6ROOT%\VC98\bin\nmake -fmsowldll.mak RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 UNICODE=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9
:------------------------------------------------------------------------------------------

:------------------------------------------------------------------------------------------
echo Compile with MS VC 6.0 static OWL6V.LIB STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmscmake RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9

echo Compile with MS VC 6.0 static OWL6VD.LIB STD RTL
"%VS6ROOT%\VC98\bin\nmake" -fmscmake RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9
:------------------------------------------------------------------------------------------

:------------------------------------------------------------------------------------------
:echo Compile with MS VC 6.0 OWL6VU.LIB STD RTL + UNICODE
:%VS6ROOT%\VC98\bin\nmake -fmscmake RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=0 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 UNICODE=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9

:echo Compile with MS VC 6.0 static OWL6DVU.LIB STD RTL + UNICODE
:%VS6ROOT%\VC98\bin\nmake -fmscmake RCDEFINES=/i..\..\include;..\..\include\owl DEBUG=1 DEBDEFS="/D__DEBUG=2 /D__TRACE /D__WARN" CODEVIEW=1 PLATFORM=INTEL WIN32=1 BI_STD_RTL=1 UNICODE=1 MBCS=0 VERSION=6 %1 %2 %3 %4 %5 %6 %7 %8 %9
:===========================================================================================

