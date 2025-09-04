@echo off
rem run_tlib.bat <objdir> <libpath>
setlocal enableextensions
if "%~1"=="" goto usage
if "%~2"=="" goto usage
set OBJDIR=%~1
set LIBPATH=%~2
if not exist "%OBJDIR%\libobjs.rsp" (
  echo ERROR: Missing response file: %OBJDIR%\libobjs.rsp
  exit /b 2
)
pushd "%OBJDIR%"
"C:\Apps\BC5\bin\tlib.exe" /P256 /0 "%LIBPATH%" @libobjs.rsp
set ERR=%ERRORLEVEL%
popd
exit /b %ERR%

:usage
echo Usage: %~nx0 ^<objdir^> ^<libpath^>
exit /b 1

