#!/bin/sh

# Run clean when the argument 'clean' is provided. Usefull for eclipse. 
if [[ $1 = clean ]]
then
	echo "Running clean"
	./clean.sh
	exit
fi

rootDir=`pwd`

if [[ $1 = ffmpeg ]]
then
	COMMAND="qmake -spec macx-g++ ${rootDir}/cppn-x-mpeg.pro"
else
	COMMAND="qmake -spec macx-g++ ${rootDir}/cppn-x-debug.pro"
fi

# Add the current directory as the root. 
# Note that this means the script will likely fail if not run from the root directory.


# Set QT variables.
# Leave blank for automatic detection (recommended)
#QT_INCLUDEDIRS=""
#QT_INCLUDEDIRS+="/Library/Frameworks/QtCore.framework/Versions/4/Headers;"
#QT_INCLUDEDIRS+="/Library/Frameworks/QtGui.framework/Versions/4/Headers"
#QT_LIBRARYDIR=""
#QT_GUI="/Library/Frameworks/QtGui.framework"
#QT_CORE="/Library/Frameworks/QtCore.framework"

# Set other variables.
# The build dir determines which directory is created to use as a build dir.
# Note that the CMakeLists file does not allow 'in-source' builds so 'buildDir' can not be '.'
buildDir="build/debug"

# Build cppn_examiner
echo Building CPPN Examiner
mkdir -p ${buildDir}
cd ${buildDir}
echo ${COMMAND}
${COMMAND}
make
cd ${rootDir}
