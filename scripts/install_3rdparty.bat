@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: install opencv
call ..\3rdparty\opencv-4.5.0\install_opencv.bat

:: install phobosIntegration
call ..\3rdparty\phobosIntegration-1.0.54\install_phobosIntegration.bat

:: reset working directory
cd %initcwd%

:: complete message
echo 3rdparty install complete.
pause

ENDLOCAL