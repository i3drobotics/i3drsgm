# I3DRSGM [FOR INTERNAL USE ONLY]
I3DR's Semi-Global Matcher for generation of disparity from stereo images. 
To use this repository you will need to have been given access to use i3drobotics private repositories. This is due to the PhobosIntegration dependency being IP owned by I3DR which should not be accesable outside of the organisation. 

## Latest Release
Download the latest release v1.0.0 [here](https://github.com/i3drobotics/i3drsgm/releases/download/v1.0.0/i3drsgm-1.0.0.exe)

# Requirements
 - cmake 
 - OpenCV v3.4.1 (See setup instructions for details.)
 - PhobosIntegration v1.0.54 (See setup instructions for details.)
 - Z-zip (Used to package library into exe, see package instructions for details.)
 - Fetch (Portable app included in this repo. Used to fetch private github release files, see setup instructions for details.)

# Compatibility
Tested on the following systems:
 - Windows 10 (x64)

# Setup
## Windows
### Automatic setup
Add a file to the scripts folder with the name 'token.txt'. Containing your GitHub public access token. 
Double click 'install_3rdparty.bat' found in the scripts folder.
This will download and install 3rd party libraries to the correct folders.

This script uses a portable version of 'fetch' to download from private github releases. Find details [here](https://github.com/gruntwork-io/fetch.git)

### Manual setup
#### OpenCV
Download OpenCV 3.4.1 from [here](https://sourceforge.net/projects/opencvlibrary/files/3.4.10/opencv-3.4.10-vc14_vc15.exe/download)
Install self extracting archive into 3rdparty/opencv-3.4.1/

#### PhobosIntegration
Downoad Phobos Integration 1.0.54 from [here](TODO). You will needed to be given access to this private repository from I3DR. Contact info@i3drobotics.com for more info.


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
Double click 'package.bat' found in the scripts folder. This will package the files in install/i3drsgm into an exe using 7-zip and get everything ready for a software release.
The script expects 7-zip to be installed at 'C:\Program Files\7-Zip'. If this is different then you will need to adapt this script. 

## Linux
COMING SOON