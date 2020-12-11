@echo off
setlocal disableDelayedExpansion
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read i3drsgm version from file
set /p version=< ..\version.txt 

set InputFile=install_i3drsgm.template
set OutputFile=install_i3drsgm.bat
set insertline=10
set newline=set version=%version%

(for /f "tokens=1* delims=[]" %%a in ('find /n /v "##" ^< "%InputFile%"') do (
if "%%~a"=="%insertline%" (
echo %newline%
REM ECHO.%%b
) ELSE (
echo.%%b
)
)) > %OutputFile%

:: reset working directory
cd %initcwd%

ENDLOCAL