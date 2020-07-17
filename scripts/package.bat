@echo off
SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: set 7z directory
set z7path="C:\Program Files\7-Zip\7z"
:: read i3drsgm version from file
set /p version=< version.txt

:: generate i3drsgm install script
call update_install_script.bat
:: copy install script to release folder
mkdir ..\release
copy install_i3drsgm.bat ..\release\install_i3drsgm.bat
del install_i3drsgm.bat
:: copy fetch applicaiton to release folder
copy ..\3rdparty\phobosIntegration-1.0.54\fetch.exe ..\release\fetch.exe /Y

cd ..\

:: copy cmake directory to release folder
xcopy cmake install\i3drsgm\cmake\ /E/H
:: copy Phobos Integration download files to release folder
mkdir install\i3drsgm\phobosIntegration-1.0.54\
copy 3rdparty\phobosIntegration-1.0.54\install_phobosIntegration.bat install\i3drsgm\phobosIntegration-1.0.54\install_phobosIntegration.bat /Y
copy 3rdparty\phobosIntegration-1.0.54\fetch.exe install\i3drsgm\phobosIntegration-1.0.54\fetch.exe /Y
:: copy OpenCV download files to release folder
mkdir install\i3drsgm\opencv-3.4.1\
copy 3rdparty\opencv-3.4.1\install_opencv.bat install\i3drsgm\opencv-3.4.1\install_opencv.bat /Y
:: create self-extracting exe
cd install
%z7path% a -sfx ../release/i3drsgm-%version%.exe i3drsgm

:: remove unneeded files from install folder
cd ..
rmdir install\i3drsgm\phobosIntegration-1.0.54 /s /q
rmdir install\i3drsgm\cmake /s /q
rmdir install\i3drsgm\opencv-3.4.1 /s /q

:: reset working directory
cd %initcwd%

:: complete message
echo packaging complete.
pause

ENDLOCAL