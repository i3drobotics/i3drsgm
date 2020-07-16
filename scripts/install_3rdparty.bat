@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%

cd ../

call 3rdparty/opencv-3.4.1/install_opencv.bat
echo 3rdparty install complete.
pause