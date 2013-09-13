#!/bin/sh
if [ ! -d "build/cppn-x.app/Contents/Frameworks" ]
then
	echo "Creating Frameworks directory..."
	mkdir build/cppn-x.app/Contents/Frameworks
fi

if [ ! -d "build/cppn-x.app/Contents/Frameworks/QtCore.framework" ]
then
	echo "Copying QtCore.framework..."
        cp -R /Library/Frameworks/QtCore.framework build/cppn-x.app/Contents/Frameworks
fi

if [ ! -d "build/cppn-x.app/Contents/Frameworks/QtGui.framework" ]
then
	echo "Copying QtGui.framework..."
        cp -R /Library/Frameworks/QtGui.framework build/cppn-x.app/Contents/Frameworks
fi

cp /usr/local/lib/libgsl.0.dylib build/cppn-x.app/Contents/MacOS
#cp /usr/lib/libiconv.2.dylib build/cppn-x.app/Contents/MacOS
#cp /usr/lib/libbz2.1.0.dylib build/cppn-x.app/Contents/MacOS

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore build/cppn-x.app/Contents/Frameworks/QtCore.framework/Versions/4.0/QtCore
install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui build/cppn-x.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui

install_name_tool -id @executable_path/libgsl.0.dylib build/cppn-x.app/Contents/MacOS/libgsl.0.dylib
#install_name_tool -id @executable_path/libiconv.2.dylib build/cppn-x.app/Contents/MacOS/libiconv.2.dylib
#install_name_tool -id @executable_path/libbz2.1.0.dylib build/cppn-x.app/Contents/MacOS/libbz2.1.0.dylib

install_name_tool -change /usr/local/lib/libgsl.0.dylib @executable_path/libgsl.0.dylib build/cppn-x.app/Contents/MacOs/cppn-x
#install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib build/cppn-x.app/Contents/MacOs/cppn-x
#install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib build/cppn-x.app/Contents/MacOs/cppn-x

install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore build/cppn-x.app/Contents/MacOs/cppn-x
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui build/cppn-x.app/Contents/MacOs/cppn-x
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore build/cppn-x.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui
