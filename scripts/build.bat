@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read i3drsgm version from file
set /p version=< version.txt  

cd ../

SET "PROJECT_SOURCE_DIR_w=%cd%"
SET "PROJECT_SOURCE_DIR=%PROJECT_SOURCE_DIR_w:\=/%"
echo %PROJECT_SOURCE_DIR%

:: make build & install folders
mkdir build
mkdir install
mkdir install\i3drsgm\i3drsgm-%version%
:: build library
cd build
cmake -G "Visual Studio 16 2019" -A x64 -DBUILD_APP=ON -DCMAKE_INSTALL_PREFIX="../install/i3drsgm/i3drsgm-%version%" -DPhobosIntegration_DIR="%PROJECT_SOURCE_DIR%/3rdparty/phobosIntegration-1.0.54" -DOpenCV_DIR="%PROJECT_SOURCE_DIR%/3rdparty/opencv-4.5.0/opencv/build" ..
:: install library
cmake --build . --config Release --target install

:: reset working directory
cd %initcwd%

:: complete message
echo Build complete.