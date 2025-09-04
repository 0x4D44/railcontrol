@echo off
rem mk_tlib_rsp.bat <objdir>
rem Generate TLIB response file listing all .obj files with + prefix.

setlocal enableextensions

if "%~1"=="" (
  echo Usage: %~nx0 ^<objdir^>
  exit /b 1
)

set OBJDIR=%~1
if not exist "%OBJDIR%" (
  echo ERROR: OBJDIR not found: %OBJDIR%
  exit /b 2
)

set RSP=%OBJDIR%\libobjs.rsp
if exist "%RSP%" del "%RSP%" >nul 2>&1

set FOUND=
pushd "%OBJDIR%" >nul
for %%f in (*.obj) do (
  set FOUND=1
  >>"%RSP%" echo +%%~nxf
)
popd >nul

if not defined FOUND (
  echo ERROR: No .obj files found in %OBJDIR%
  exit /b 3
)

endlocal & exit /b 0
