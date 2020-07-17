@echo off
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: get GitHub PAT from first argument
set token=%1
:: download phobos integration exe from github release using token
fetch --repo="https://github.com/i3drobotics/phobosIntegration" --tag="v1.0.54" --release-asset="phobosIntegration-1.0.54-win64_archive.exe" --progress --github-oauth-token=%token% ./
:: extract self-exracting archive
phobosIntegration-1.0.54-win64_archive.exe -o"./" -y
:: delete downloaded file
del phobosIntegration-1.0.54-win64_archive.exe
echo Phobos Integration install complete

:: reset working directory
cd %initcwd%