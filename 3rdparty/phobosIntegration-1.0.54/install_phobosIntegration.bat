@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd "%scriptpath:~0,-1%"

:: url for downloading opencv
SET url="https://github.com/i3drobotics/phobosIntegration/releases/download/v1.0.54/phobosIntegration-1.0.54-win64_archive.exe"
:: output folder where donwload is placed
SET "outputfolder=%cd%"
:: full output filepath
SET "output=%outputfolder%\phobosIntegration-1.0.54-win64_archive.exe"
:: download opencv from url
curl -o "%output%" -L %url%
:: extract self-exracting archive
"%output%" -o"%outputfolder%" -y
:: delete downloaded file
del "%output%"
echo Phobos Integration install complete

:: reset working directory
cd "%initcwd%"

ENDLOCAL