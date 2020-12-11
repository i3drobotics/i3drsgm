@echo off
SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read i3drsgm version from file
set /p version=< ..\version.txt 
:: set 7z directory
set z7path="C:\Program Files\7-Zip\7z"

:: generate i3drsgm install script
call update_install_script.bat
:: copy install script to release folder
mkdir ..\release
copy install_i3drsgm.bat ..\release\install_i3drsgm.bat
del install_i3drsgm.bat

cd ..\

:: copy cmake directory to release folder
xcopy cmake install\i3drsgm\cmake\ /E/H
:: copy Phobos Integration download files to release folder
mkdir install\i3drsgm\phobosIntegration-1.0.54\
copy 3rdparty\phobosIntegration-1.0.54\install_phobosIntegration.bat install\i3drsgm\phobosIntegration-1.0.54\install_phobosIntegration.bat /Y
:: copy OpenCV download files to release folder
mkdir install\i3drsgm\opencv-4.5.0\
copy 3rdparty\opencv-4.5.0\install_opencv.bat install\i3drsgm\opencv-4.5.0\install_opencv.bat /Y
:: move bin folder from i3drsgm
mkdir install\i3drsgm_app
xcopy install\i3drsgm\i3drsgm-%version%\bin\ install\i3drsgm_app\ /E/H
rmdir install\i3drsgm\i3drsgm-%version%\bin /s /q
:: create self-extracting exe for library
cd install
%z7path% a -sfx ../release/i3drsgm-%version%.exe i3drsgm
:: create self-extracting exe for app
%z7path% a ../release/i3drsgm-%version%-app.zip i3drsgm_app
cd ..

:: reset install folder to original state
xcopy install\i3drsgm_app\ install\i3drsgm\i3drsgm-%version%\bin\ /E/H
rmdir install\i3drsgm_app\ /s /q
rmdir install\i3drsgm\cmake\ /s /q
rmdir install\i3drsgm\opencv-4.5.0\ /s /q
rmdir install\i3drsgm\phobosIntegration-1.0.54\ /s /q

:: reset working directory
cd %initcwd%

:: complete message
echo packaging complete.

ENDLOCAL