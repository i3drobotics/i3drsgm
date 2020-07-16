@echo off
SET scriptpath=%~dp0
echo %scriptpath:~0,-1%
cd %scriptpath:~0,-1%
set z7path="C:\Program Files\7-Zip\7z"
set version="1.0.0"
cd install
%z7path% a -sfx ../installer/i3drsgm-%version%.exe i3drsgm
echo "packaging complete."
pause