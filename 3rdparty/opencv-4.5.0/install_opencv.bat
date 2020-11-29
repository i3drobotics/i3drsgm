@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: url for downloading opencv
SET url="https://github.com/opencv/opencv/releases/download/4.5.0/opencv-4.5.0-vc14_vc15.exe"
:: output folder where donwload is placed
SET "outputfolder=%cd%"
:: full output filepath
SET "output=%outputfolder%\opencv-4.5.0-vc14_vc15.exe"
:: download opencv from url
curl -o "%output%" -L %url%
:: extract self-exracting archive
%output% -o%outputfolder% -y
:: delete downloaded file
del %output%
echo OpenCV install complete.

:: reset working directory
cd %initcwd%

ENDLOCAL