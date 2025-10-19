@echo off
setlocal

if defined VSINSTALLDIR (
  set "VSROOT=%VSINSTALLDIR%"
) else (
  for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSROOT=%%i"
)

if not defined VSROOT (
  echo Error: Unable to locate a Visual Studio installation with MSVC tools.
  exit /b 1
)

call "%VSROOT%\VC\Auxiliary\Build\vcvarsall.bat" x86 || exit /b 1

for %%I in ("%~dp0..\..") do set "ROOT=%%~fI"

call :build owlcore || goto :fail
call :build owlext || goto :fail
call :build ocf || goto :fail
call :build coolprj || goto :fail

echo OWLNext build complete.
exit /b 0

:build
pushd "%ROOT%\third_party\owlnext\source\%1" >nul || exit /b 1
nmake -f vc.mak COMPAT=5
set "ERR=%ERRORLEVEL%"
popd >nul
exit /b %ERR%

:fail
exit /b 1
