@echo off
:: get GitHub PAT from first argument
set token=%1
:: download phobos integration exe from github release using token
fetch --repo="https://github.com/i3drobotics/phobosIntegration" --tag="v1.0.54" --release-asset="phobosIntegration-1.0.54-win64_archive.exe" --progress --github-oauth-token=%token% ./
:: extract self-exracting archive
phobosIntegration-1.0.54-win64_archive.exe -o"./" -y
:: delete downloaded file
del phobosIntegration-1.0.54-win64_archive.exe