@echo off

:: set working directory to script directory
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%

:: read github PAT from text file
set /p token=< token.txt  

:: install opencv
call ..\3rdparty\opencv-3.4.1\install_opencv.bat

:: install phobosIntegration
cd ..\3rdparty\phobosIntegration-1.0.54\
call install_phobosIntegration.bat %token%

:: reset working directory
cd ..\..\

:: complete message
echo 3rdparty install complete.
pause