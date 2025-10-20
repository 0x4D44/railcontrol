@echo off
setlocal

REM Compatibility wrapper to call the main build script
call "%~dp0build.cmd" %*
exit /b %ERRORLEVEL%

