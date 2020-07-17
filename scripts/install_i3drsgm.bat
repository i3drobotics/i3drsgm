@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%
SET url="TODO"
SET outputfolder="%cd%"
SET output="%outputfolder%\i3drsgm-v1.0.1.exe"
bitsadmin /transfer OpenCV_Download_Job /download /priority normal %url% %output%
%output% -o%outputfolder% -y
del %output%
echo opencv install complete.