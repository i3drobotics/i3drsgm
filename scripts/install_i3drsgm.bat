@echo off
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: get GitHub PAT from first argument
set token=%1
:: download phobos integration exe from github release using token
fetch --repo="https://github.com/i3drobotics/i3drsgm" --tag="v1.0.1" --release-asset="i3drsgm-1.0.1.exe" --progress --github-oauth-token=%token% ./
:: extract self-exracting archive
i3drsgm-1.0.1.exe -o"./" -y
:: delete downloaded file
del i3drsgm-1.0.1.exe

:: complete message
echo I3DRSGM install complete

:: reset working directory
cd %initcwd%