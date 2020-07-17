@echo off
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: set i3drsgm version
set version="1.0.1"

:: get GitHub PAT from first argument
set token=%1
:: download phobos integration exe from github release using token
fetch --repo="https://github.com/i3drobotics/i3drsgm" --tag="v%version%" --release-asset="i3drsgm-%version%.exe" --progress --github-oauth-token=%token% ./
:: extract self-exracting archive
i3drsgm-%version%.exe -o"./" -y
:: delete downloaded file
del i3drsgm-%version%.exe

:: complete message
echo I3DRSGM install complete

:: reset working directory
cd %initcwd%