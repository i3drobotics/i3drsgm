@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read github PAT from text file
set /p token=< token.txt  

:: install opencv
call ..\3rdparty\opencv-3.4.1\install_opencv.bat

:: install phobosIntegration
call ..\3rdparty\phobosIntegration-1.0.54\install_phobosIntegration.bat %token%

:: reset working directory
cd %initcwd%

:: complete message
echo 3rdparty install complete.
pause

ENDLOCAL