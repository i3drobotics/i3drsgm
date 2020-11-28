@echo off

SETLOCAL
:: set working directory to script directory
SET initcwd=%cd%
SET scriptpath=%~dp0
cd %scriptpath:~0,-1%

:: read i3drsgm version from file
set /p version=< version.txt  

cd ../py/PyI3DRSGM

python -m pip install --user --upgrade twine wheel
python setup.py sdist bdist_wheel

:: reset working directory
cd %initcwd%

:: complete message
echo Python build complete.