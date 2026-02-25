@echo off
setlocal
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT (
  echo Error: VS not found.
  exit /b 1
)
call "%VSROOT%\VC\Auxiliary\Build\vcvarsall.bat" x86 || exit /b 1

rem Build core lib and tests only
msbuild build\msvc\RailCore.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32 /p:Restore=False /p:RestorePackages=false /p:RestoreProjectStyle=None || exit /b 1
msbuild build\msvc\RailCoreTests.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32 /p:Restore=False /p:RestorePackages=false /p:RestoreProjectStyle=None || exit /b 1

if /I "%CONFIG%"=="Debug" (
  if exist build\msvc\Debug\RailCoreTests.exe build\msvc\Debug\RailCoreTests.exe
)

exit /b 0

