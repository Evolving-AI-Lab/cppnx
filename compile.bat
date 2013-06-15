@echo off
call qt-path.bat
call qtvars.bat
echo PATH
mkdir build
cd build
qmake -spec win32-g++ ../cppn-x.pro
call make release
cd ..
