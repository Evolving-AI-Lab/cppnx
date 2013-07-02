@echo off
call qt-path.bat
set PATH=%PATH%;%QT_SHARED_PATH%\bin
call "build/release/cppn-x.exe"