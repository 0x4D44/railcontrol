@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT=%%~fI"
set "SOURCE=%ROOT%\tools\lint\memory_guard_stub.cpp"

if not exist "%SOURCE%" (
  echo Error: lint stub not found at %SOURCE%.
  exit /b 1
)

if defined VSINSTALLDIR (
  set "VSROOT=%VSINSTALLDIR%"
) else (
  for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSROOT=%%i"
)

if not defined VSROOT (
  echo Error: Unable to locate a Visual Studio installation with MSVC tools.
  exit /b 1
)

call "%VSROOT%\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul
if errorlevel 1 (
  echo Error: Failed to initialise MSVC environment.
  exit /b %ERRORLEVEL%
)

set "TMP_OBJ=%TEMP%\memory_guard_stub.obj"
if exist "%TMP_OBJ%" del /f /q "%TMP_OBJ%" >nul 2>&1

pushd "%ROOT%" >nul
cl /nologo /c /W4 /WX /std:c++17 /EHsc /Zc:wchar_t /permissive- /U_MSC_VER /I"%ROOT%" /Fo"%TMP_OBJ%" "%SOURCE%"
set "RESULT=%ERRORLEVEL%"
popd >nul

if exist "%TMP_OBJ%" del /f /q "%TMP_OBJ%" >nul 2>&1

exit /b %RESULT%
