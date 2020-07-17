@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read i3drsgm version from file
set /p version=< version.txt  

cd ../

:: make build & install folders
mkdir build
mkdir install
mkdir install\i3drsgm\i3drsgm-%version%
:: build library
cd build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="..\install\i3drsgm\i3drsgm-%version%" -DPhobosIntegration_DIR="%cd%\..\3rdparty\phobosIntegration-1.0.54" -DOpenCV_DIR="%cd%\..\3rdparty\opencv-3.4.1\opencv\build" ..
:: install library
cmake --build . --config Release --target install

:: reset working directory
cd %initcwd%

:: complete message
echo Build complete.

pause