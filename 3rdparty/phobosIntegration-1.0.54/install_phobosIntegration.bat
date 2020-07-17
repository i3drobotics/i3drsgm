@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: search for token in first argument
set token_found=false
if "%~1"=="" (
    :: token not stated in argument, look for it in token.txt file
    echo Reading PAT from 'token.txt'
    if exist "token.txt" (
        :: read github PAT from text file
        set /p token=< token.txt
        set token_found=true
    ) else (
        :: failed to file token.txt file
        echo Token file not found
    )
) else (
    :: token found in first argument, read token from argument
    set token=%~1
    set token_found=true
)

:: check token was found
if %token_found%==true (
    :: download phobos integration exe from github release using token
    fetch --repo="https://github.com/i3drobotics/phobosIntegration" --tag="v1.0.54" --release-asset="phobosIntegration-1.0.54-win64_archive.exe" --progress --github-oauth-token=%token% ./
    :: extract self-exracting archive
    phobosIntegration-1.0.54-win64_archive.exe -o"./" -y
    :: delete downloaded file
    del phobosIntegration-1.0.54-win64_archive.exe
    echo Phobos Integration install complete
) else (
    echo Token not found
)

:: reset working directory
cd %initcwd%

ENDLOCAL