@echo off

:: read i3drsgm version from file
set /p version=< ..\version.txt 

:: Run application with test data
..\install\i3drsgm\i3drsgm-%version%\bin\I3DRSGMApp.exe ..\resources\left.png ..\resources\right.png ..\resources\left.yaml ..\resources\right.yaml ..\resources\output 0 1