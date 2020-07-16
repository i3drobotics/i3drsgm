@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%
SET url="https://netix.dl.sourceforge.net/project/opencvlibrary/3.4.10/opencv-3.4.10-vc14_vc15.exe"
SET outputfolder="%cd%\3rdparty\opencv-3.4.1"
SET output="%outputfolder%\opencv-3.4.10-vc14_vc15.exe"
bitsadmin /transfer OpenCV_Download_Job /download /priority normal %url% %output%
%output% -o%outputfolder% -y
echo "3rdparty install complete."
pause