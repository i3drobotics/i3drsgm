# I3DRSGM
I3DR's Semi-Global Matcher for generation of disparity from stereo images.

See [here](https://github.com/i3drobotics/i3drsgmApp) for a demo app that uses this library. 

## Status
![Build](https://github.com/i3drobotics/i3drsgm/workflows/Build/badge.svg?event=push)

## Latest Release
Download the latest release v1.0.9 [here](https://github.com/i3drobotics/i3drsgm/releases/download/v1.0.9/i3drsgm-1.0.9.exe)

# Requirements
 - cmake 
 - OpenCV v4.5.0 (See setup instructions for details.)
 - PhobosIntegration v1.0.54 (See setup instructions for details.)
 - Z-zip (Used to package library into exe, see package instructions for details.)

# Compatibility
Tested on the following systems:
 - Windows 10 (x64)

# Setup
## Windows
### Automatic setup
Double click 'install_3rdparty.bat' found in the scripts folder.
This will download and install 3rd party libraries to the correct folders.

### Manual setup
#### OpenCV
Download OpenCV 4.5.0 from [here](https://sourceforge.net/projects/opencvlibrary/files/4.5.0/opencv-4.5.0-vc14_vc15.exe/download)
Install self extracting archive into 3rdparty/opencv-4.5.0/

#### PhobosIntegration
Downoad Phobos Integration 1.0.54 from [here](https://github.com/i3drobotics/phobosIntegration/releases/download/v1.0.54/phobosIntegration-1.0.54-win64_archive.exe).

## Linux
COMING SOON

# Build
## Windows
### Automatic build
Double click 'build.bat' found in the scripts folder. This will build and install the library to 'build' and 'install' folders in the repos directory.

### Manual build
```
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_INSTALL_PREFIX="..\install" -DPhobosIntegration_DIR="%cd%\..\3rdparty\phobosIntegration-1.0.54" -DOpenCV_DIR="%cd%\..\3rdparty\opencv-4.5.0\opencv\build" ..
cmake --build . --config Release --target install
```

## Linux
### Automatic build
COMING SOON

### Manual build
```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="..\install" -DPhobosIntegration_DIR="%cd%\..\3rdparty\phobosIntegration-1.0.54" ..
make
```

# Package
## Windows
Double click 'package.bat' found in the scripts folder. This will package the files in install/i3drsgm into an exe using 7-zip and get everything ready for a software release.
The script expects 7-zip to be installed at 'C:\Program Files\7-Zip'. If this is different then you will need to adapt this script. 

## Linux
COMING SOON
