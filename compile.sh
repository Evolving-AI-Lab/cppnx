#!/bin/sh

# Run clean when the argument 'clean' is provided. Usefull for eclipse. 
if [[ $1 = clean ]]
then
	echo "Running clean"
	./clean.sh
	exit
fi

# Add the current directory as the root. 
# Note that this means the script will likely fail if not run from the root directory.
rootDir=`pwd`

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
buildDir="build"

# The JGTL inlucde dir (JGTL is a header only library, so that is all what is important).
# Only change if you want to use a different JGTL implementation.
# Use the cmake arguments below for more control over library and inlcude-file locations
#JGTL_include=${rootDir}/JGTL/include

# Build cppn_examiner
echo Building CPPN Examiner
mkdir -p ${buildDir}
cd ${buildDir}
echo qmake -spec macx-g++ ${rootDir}/cppn-x.pro
qmake -spec macx-g++ ${rootDir}/cppn-x.pro
make
cd ${rootDir}
