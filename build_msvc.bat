@echo off
setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

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

msbuild build\msvc\RailControl.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32
exit /b %ERRORLEVEL%
