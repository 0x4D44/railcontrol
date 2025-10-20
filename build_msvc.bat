@echo off
setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

set "ROOT=%~dp0"
set "OWLLIBDIR=%ROOT%third_party\owlnext\lib"
set "OWLLIB_OWL=%OWLLIBDIR%\owl-7.0-v1930-x86-5t.lib"
set "OWLLIB_EXT=%OWLLIBDIR%\owlext-7.0-v1930-x86-5t.lib"
set "OWLLIB_OCF=%OWLLIBDIR%\ocf-7.0-v1930-x86-5t.lib"
set "OWLLIB_COOL=%OWLLIBDIR%\coolprj-7.0-v1930-x86-5t.lib"

if not exist "%OWLLIB_OWL%" (
  call "%ROOT%build\msvc\build_owlnext_msvc.bat" || exit /b 1
) else if not exist "%OWLLIB_EXT%" (
  call "%ROOT%build\msvc\build_owlnext_msvc.bat" || exit /b 1
) else if not exist "%OWLLIB_OCF%" (
  call "%ROOT%build\msvc\build_owlnext_msvc.bat" || exit /b 1
) else if not exist "%OWLLIB_COOL%" (
  call "%ROOT%build\msvc\build_owlnext_msvc.bat" || exit /b 1
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

call "%VSROOT%\VC\Auxiliary\Build\vcvarsall.bat" x86 || exit /b 1

msbuild build\msvc\RailControl.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32
if errorlevel 1 exit /b %ERRORLEVEL%

call :copy_dbghelp "%CONFIG%"
exit /b %ERRORLEVEL%

:copy_dbghelp
set "TARGET_CONFIG=%~1"
set "TARGET_DIR=%ROOT%build\msvc\%TARGET_CONFIG%"
if not exist "%TARGET_DIR%" exit /b 0

set "DBGHELP_SRC="

set "SDKROOT=%WindowsSdkDir%"
set "SDKVER=%WindowsSDKVersion%"
if defined SDKROOT (
  if "%SDKROOT:~-1%"=="\" set "SDKROOT=%SDKROOT:~0,-1%"
)
if defined SDKVER (
  if "%SDKVER:~-1%"=="\" set "SDKVER=%SDKVER:~0,-1%"
)

if defined SDKROOT (
  if defined SDKVER (
    if exist "%SDKROOT%bin\%SDKVER%\x86\dbghelp.dll" set "DBGHELP_SRC=%SDKROOT%bin\%SDKVER%\x86\dbghelp.dll"
  )
  if not defined DBGHELP_SRC (
    if exist "%SDKROOT%bin\x86\dbghelp.dll" set "DBGHELP_SRC=%SDKROOT%bin\x86\dbghelp.dll"
  )
)

if not defined DBGHELP_SRC (
  if exist "%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x86\dbghelp.dll" set "DBGHELP_SRC=%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x86\dbghelp.dll"
)

if not defined DBGHELP_SRC (
  if exist "%ProgramFiles%\Windows Kits\10\Debuggers\x86\dbghelp.dll" set "DBGHELP_SRC=%ProgramFiles%\Windows Kits\10\Debuggers\x86\dbghelp.dll"
)

if defined DBGHELP_SRC (
  copy /Y "%DBGHELP_SRC%" "%TARGET_DIR%\dbghelp.dll" >nul
)

exit /b 0
