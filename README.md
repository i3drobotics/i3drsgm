# I3DRSGM
I3DR's Semi-Global Matcher for generation of disparity from stereo images. 

## Latest Release
Download the latest release v1.0.0 [here]()

# Requirements
 - cmake 
 - OpenCV (See setup instructions for details)
 - Z-zip (Used to package library into exe, see package instructions for details)

# Tested
Tested on the following systems:
 - Windows 10 (x64)

# Setup
## Windows
### Automatic setup
Double click 'install_3rdparty.bat' found in the scripts folder.
This will download and install 3rd party libraries to the correct folders.

### Manual setup
#### OpenCV
Download OpenCV 3.4.1 from [here](https://sourceforge.net/projects/opencvlibrary/files/3.4.10/opencv-3.4.10-vc14_vc15.exe/download)
Install self extracting archive into 3rdparty/opencv-3.4.1/

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
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="..\install" -DPhobosIntegration_DIR="%cd%\..\3rdparty\phobosIntegration-1.0.54" -DOpenCV_DIR="%cd%\..\3rdparty\opencv-3.4.1\opencv\build" ..
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
Double click 'package.bat' found in the scripts folder. This will package the files in install/i3drsgm into an exe using 7-zip.
The script expects 7-zip to be installed at 'C:\Program Files\7-Zip'. If this is different then you will need to adapt this script. 

## Linux
COMING SOON