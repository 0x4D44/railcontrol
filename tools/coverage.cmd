@echo off
setlocal

set "TEST_EXE=%~1"
set "OUT_XML=%~2"
if "%TEST_EXE%"=="" (
  echo Usage: coverage.cmd ^<path-to-test-exe^> ^<path-to-output-xml^>
  exit /b 2
)
if not exist "%TEST_EXE%" (
  echo Error: Test executable not found: %TEST_EXE%
  exit /b 3
)

set "OCC_BIN=OpenCppCoverage.exe"

rem Try find OpenCppCoverage on PATH first
where "%OCC_BIN%" >nul 2>&1
if errorlevel 1 (
  rem Try common Chocolatey location
  if exist "%ProgramData%\chocolatey\bin\OpenCppCoverage.exe" set "OCC_BIN=%ProgramData%\chocolatey\bin\OpenCppCoverage.exe"
)
if not exist "%OCC_BIN%" (
  rem Try Program Files
  if exist "%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe" set "OCC_BIN=%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe"
)

if not exist "%OCC_BIN%" (
  echo OpenCppCoverage not found. Install via Chocolatey: ^"choco install opencppcoverage^" or put OpenCppCoverage.exe on PATH.
  echo Skipping coverage run; executing tests normally.
  "%TEST_EXE%"
  exit /b %ERRORLEVEL%
)

echo Using OpenCppCoverage: %OCC_BIN%
set "ROOT=%~dp0..\"
for %%I in ("%TEST_EXE%") do set "TEST_DIR=%%~dpI"
if not defined TEST_DIR set "TEST_DIR=%CD%\"
pushd "%TEST_DIR%" >nul

"%OCC_BIN%" --quiet --cover_children ^
  --export_type=cobertura:"%OUT_XML%" ^
  --sources="%ROOT%src\railcore" ^
  --excluded_sources="%ROOT%tests" ^
  --excluded_sources="%ROOT%third_party" ^
  --excluded_sources="%ROOT%src\railui" ^
  -- "%TEST_EXE%"

set "_RC=%ERRORLEVEL%"
popd >nul
if not defined _RC set "_RC=0"
exit /b %_RC%

