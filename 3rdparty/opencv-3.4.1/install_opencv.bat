@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: url for downloading opencv
SET url="https://netix.dl.sourceforge.net/project/opencvlibrary/3.4.10/opencv-3.4.10-vc14_vc15.exe"
:: output folder where donwload is placed
SET outputfolder="%cd%"
:: full output filepath
SET output="%outputfolder%\opencv-3.4.10-vc14_vc15.exe"
:: download opencv from url
bitsadmin /transfer OpenCV_Download_Job /download /priority normal %url% %output%
:: extract self-exracting archive
%output% -o%outputfolder% -y
:: delete downloaded file
del %output%
echo OpenCV install complete.

:: reset working directory
cd %initcwd%

ENDLOCAL