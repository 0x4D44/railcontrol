@echo off
setlocal

set "ACTION=%~1"
set "CONFIG=%~2"

if "%ACTION%"=="" set "ACTION=enable"
if "%CONFIG%"=="" set "CONFIG=Debug"

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT=%%~fI"
set "EXE=%ROOT%\build\msvc\%CONFIG%\railc_msvc.exe"

if exist "%ROOT%\build\msvc\%CONFIG%\railc.exe" (
  set "EXE=%ROOT%\build\msvc\%CONFIG%\railc.exe"
)

if not exist "%EXE%" (
  echo Error: Unable to find target executable for configuration "%CONFIG%".
  echo Expected to find "%EXE%".
  goto :usage
)

if /I "%ACTION%"=="enable" (
  call :ensure_tool gflags.exe || exit /b 1
  call :ensure_tool appverif.exe || exit /b 1

  echo Enabling PageHeap (gflags) for "%EXE%"
  gflags.exe /p /enable "%EXE%" /full || exit /b 1

  echo Enabling Application Verifier (Heaps+Handles+Exceptions) for "%EXE%"
  appverif.exe /verify "%EXE%" /tests Heaps,Handles,Exceptions || exit /b 1

  echo Memory guards enabled for %CONFIG%.
  goto :status
)

if /I "%ACTION%"=="disable" (
  call :ensure_tool gflags.exe || exit /b 1
  call :ensure_tool appverif.exe || exit /b 1

  echo Disabling PageHeap (gflags) for "%EXE%"
  gflags.exe /p /disable "%EXE%" || exit /b 1

  echo Disabling Application Verifier for "%EXE%"
  appverif.exe /unverify "%EXE%" || exit /b 1

  echo Memory guards disabled for %CONFIG%.
  goto :status
)

if /I "%ACTION%"=="status" (
  goto :status
)

:usage
echo.
echo Usage:
echo   enable_memory_guards.bat ^<enable^|disable^|status^> [Debug^|Release]
echo.
echo Examples:
echo   enable_memory_guards.bat enable Debug
echo   enable_memory_guards.bat disable Release
echo   enable_memory_guards.bat status
exit /b 1

:status
call :ensure_tool gflags.exe || exit /b 1
call :ensure_tool appverif.exe || exit /b 1

echo.
echo ==== PageHeap status (gflags) ====
gflags.exe /p /query "%EXE%"

echo.
echo ==== Application Verifier status ====
appverif.exe /query "%EXE%"

exit /b %ERRORLEVEL%

:ensure_tool
where /q "%~1"
if errorlevel 1 (
  echo Error: "%~1" not found on PATH. Install the Windows SDK Debugging Tools and ensure gflags/appverif are accessible.
  exit /b 1
)
exit /b 0
