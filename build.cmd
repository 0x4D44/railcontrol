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

msbuild build\msvc\RailControl.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32 /p:Restore=False /p:RestorePackages=false /p:RestoreProjectStyle=None
if errorlevel 1 exit /b %ERRORLEVEL%
if not exist "%ROOT%build\msvc\%CONFIG%\railc.exe" (
  echo Error: Build succeeded but railc.exe was not produced.
  exit /b 1
)

msbuild build\msvc\RailCore.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32 /p:Restore=False /p:RestorePackages=false /p:RestoreProjectStyle=None
if errorlevel 1 exit /b %ERRORLEVEL%
if not exist "%ROOT%build\msvc\%CONFIG%\RailCore.lib" (
  echo Error: Build succeeded but RailCore.lib was not produced.
  exit /b 1
)

msbuild build\msvc\RailCoreTests.vcxproj /p:Configuration=%CONFIG% /p:Platform=Win32 /p:Restore=False /p:RestorePackages=false /p:RestoreProjectStyle=None
if errorlevel 1 exit /b %ERRORLEVEL%
if not exist "%ROOT%build\msvc\%CONFIG%\RailCoreTests.exe" (
  echo Error: Build succeeded but RailCoreTests.exe was not produced.
  exit /b 1
)

rem Optionally build GoogleTest-based tests (opt-in locally via BUILD_GTEST=1)
if /I "%BUILD_GTEST%"=="1" (
  echo Building RailCoreGTest with NuGet restore...
  msbuild build\msvc\RailCoreGTest.vcxproj /restore /p:Configuration=%CONFIG% /p:Platform=Win32
)

rem Optionally build GoogleTest-based tests (if available) - disabled by default in local script

if /I "%CONFIG%"=="Debug" (
  set "RAILCORE_SMOKE_MODE=minimal"
  set "OUTDIR=%ROOT%build\msvc\%CONFIG%"
  if exist "%ROOT%build\msvc\Debug\RailCoreTests.exe" (
    if /I "%COVERAGE%"=="1" (
      echo Running coverage for RailCoreTests...
      call "%ROOT%tools\coverage.cmd" "%ROOT%build\msvc\Debug\RailCoreTests.exe" "%ROOT%build\msvc\Debug\coverage.xml"
    ) else (
      echo Running RailCoreTests...
      powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command "$p=Start-Process -FilePath '%ROOT%build\\msvc\\Debug\\RailCoreTests.exe' -WorkingDirectory '%ROOT%build\\msvc\\Debug' -PassThru -Wait; exit $p.ExitCode"
      if errorlevel 1 (
        if /I "%STRICT%"=="1" (
          echo RailCoreTests failed and STRICT=1 set; failing build.
          exit /b 1
        ) else (
          echo RailCoreTests failed ^(tolerated; set STRICT=1 to fail the build^)
        )
      )
    )
  )
  if exist "%ROOT%build\msvc\Debug\RailCoreGTest.exe" (
    if /I "%COVERAGE%"=="1" (
      echo Running coverage for RailCoreGTest...
      call "%ROOT%tools\coverage.cmd" "%ROOT%build\msvc\Debug\RailCoreGTest.exe" "%ROOT%build\msvc\Debug\coverage_gtest.xml"
    ) else (
      echo Running RailCoreGTest...
      powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command "$p=Start-Process -FilePath '%ROOT%build\\msvc\\Debug\\RailCoreGTest.exe' -ArgumentList '--gtest_color=no','--gtest_output=xml:%ROOT%build\\msvc\\Debug\\gtest-results.xml' -WorkingDirectory '%ROOT%build\\msvc\\Debug' -PassThru -Wait; exit $p.ExitCode"
      if errorlevel 1 (
        if /I "%STRICT%"=="1" (
          echo RailCoreGTest failed and STRICT=1 set; failing build.
          exit /b 1
        ) else (
          echo RailCoreGTest failed ^(tolerated; set STRICT=1 to fail the build^)
        )
      )
    )
  )
)

call :copy_dbghelp "%CONFIG%"
call :copy_help "%CONFIG%"

set "OUTDIR=%ROOT%build\msvc\%CONFIG%"
if exist "%OUTDIR%\railc.exe" copy /Y "%OUTDIR%\railc.exe" "%ROOT%railc.exe" >nul
if exist "%OUTDIR%\RAILC.HLP" copy /Y "%OUTDIR%\RAILC.HLP" "%ROOT%RAILC.HLP" >nul

exit /b 0

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
if defined DBGHELP_SRC copy /Y "%DBGHELP_SRC" "%TARGET_DIR%\dbghelp.dll" >nul
exit /b 0

:copy_help
set "TARGET_CONFIG=%~1"
set "TARGET_DIR=%ROOT%build\msvc\%TARGET_CONFIG%"
if not exist "%TARGET_DIR%" exit /b 0
if exist "%ROOT%HELP\RAILC.HLP" copy /Y "%ROOT%HELP\RAILC.HLP" "%TARGET_DIR%\RAILC.HLP" >nul
exit /b 0



