@echo off
call qt-path.bat
call qtvars.bat
mkdir build
cd build
call qmake "QT_DEV_PATH=%QT_DEV_PATH%" -spec win32-g++ ../cppn-x-mpeg.pro
call make release
cd ..
