@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%
set z7path="C:\Program Files\7-Zip\7z"
set version="1.0.1"

cd ../

xcopy cmake install\i3drsgm\cmake\ /E/H
xcopy 3rdparty\phobosIntegration-1.0.54 install\i3drsgm\phobosIntegration-1.0.54\ /E/H
mkdir install\i3drsgm\opencv-3.4.1\
copy 3rdparty\opencv-3.4.1\install_opencv.bat install\i3drsgm\opencv-3.4.1\install_opencv.bat /Y
cd install
%z7path% a -sfx ../installer/i3drsgm-%version%.exe i3drsgm
cd ..
rmdir install\i3drsgm\phobosIntegration-1.0.54 /s /q
rmdir install\i3drsgm\cmake /s /q
rmdir install\i3drsgm\opencv-3.4.1 /s /q

echo packaging complete.
pause