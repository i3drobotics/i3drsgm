@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%
set version="1.0.1"

cd ../

mkdir build
mkdir install
mkdir install\i3drsgm\i3drsgm-%version%
cd build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="..\install\i3drsgm\i3drsgm-%version%" -DPhobosIntegration_DIR="%cd%\..\3rdparty\phobosIntegration-1.0.54" -DOpenCV_DIR="%cd%\..\3rdparty\opencv-3.4.1\opencv\build" ..
cmake --build . --config Release --target install
cd ..
echo Build complete.
pause