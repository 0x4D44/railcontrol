@echo off
rem add_objs_tlib.bat <objdir> <libpath>
setlocal enableextensions
if "%~1"=="" goto usage
if "%~2"=="" goto usage
set OBJDIR=%~1
set LIBPATH=%~2
pushd "%OBJDIR%" || exit /b 2
for %%f in (*.obj) do (
  "C:\Apps\BC5\bin\tlib.exe" /P256 /0 "%LIBPATH%" +%%f >nul
  if errorlevel 1 goto :tlib_err
)
popd
exit /b 0

:tlib_err
set ERR=%ERRORLEVEL%
popd
exit /b %ERR%

:usage
echo Usage: %~nx0 ^<objdir^> ^<libpath^>
exit /b 1

