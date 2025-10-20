@echo off
setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

REM Invoke MSVC build, then stage railc.exe in repo root
call "%~dp0build_msvc.bat" "%CONFIG%" || exit /b 1

set "OUTDIR=%~dp0build\msvc\%CONFIG%"
if exist "%OUTDIR%\railc.exe" copy /Y "%OUTDIR%\railc.exe" "%~dp0railc.exe" >nul

exit /b 0

