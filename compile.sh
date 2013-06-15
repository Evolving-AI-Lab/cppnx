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


# Set BOOST variables.
# Note that the boost LIBRARYDIR variable itself will not be used if every library is supplied with its absolute path.
# Leave BOOST_LIBRARIES blank for automattic detection of a boost library if possible (recommended)
#boost_lib_ext=dylib
#BOOST_INCLUDEDIR=/usr/local/include
#BOOST_LIBRARYDIR=/usr/local/lib
#BOOST_LIBRARIES=""
#BOOST_LIBRARIES+="${BOOST_LIBRARYDIR}/libboost_filesystem.${boost_lib_ext};"
#BOOST_LIBRARIES+="${BOOST_LIBRARYDIR}/libboost_system.${boost_lib_ext};"
#BOOST_LIBRARIES+="${BOOST_LIBRARYDIR}/libboost_thread.${boost_lib_ext};"
#BOOST_LIBRARIES+="${BOOST_LIBRARYDIR}/libboost_regex.${boost_lib_ext}"

#BOOST_LIBRARIES+="boost_filesystem;"
#BOOST_LIBRARIES+="boost_system;"
#BOOST_LIBRARIES+="boost_thread;"
#BOOST_LIBRARIES+="boost_regex"

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

# The zlib root dir.
# Only change if you want to use a different zlib library.
# You will have to change the library path in the CMAKE_ARGUMENTS below to link to a different library (not tested).
#zlib_root=${rootDir}/zlib

# The NEAT root dir.
# Only change if you want to use a different NEAT implementation.
# Note that the supplied NEAT imlementation is slighlty altered, 
# and using a different one will likely produce unexpected result.
# You will have to change the library path in the CMAKE_ARGUMENTS below to link to a different library (not tested).
#NEAT_root=${rootDir}/NEAT

# The TinyXML-DDL root dir.
# Only change if you want to use a different TinyXML implementation.
# You will have to change the library path in the CMAKE_ARGUMENTS below to link to a different library (not tested).
#tinyXML_root=${rootDir}/tinyxmldll

# The JGTL inlucde dir (JGTL is a header only library, so that is all what is important).
# Only change if you want to use a different JGTL implementation.
# Use the cmake arguments below for more control over library and inlcude-file locations
#JGTL_include=${rootDir}/JGTL/include

# Set CMAKE arguments
# Change in which folder the executable will be placed
#CMAKE_ARGUMENTS=""
#CMAKE_ARGUMENTS+="-DEXECUTABLE_OUTPUT_PATH=${rootDir}/out "

# Set BOOST
# Use the Boost variables above to set boost libraries
#CMAKE_ARGUMENTS+="-DBoost_LIBRARY_DIRS=${BOOST_LIBRARYDIR} "
#CMAKE_ARGUMENTS+="-DBoost_INCLUDE_DIRS=${BOOST_INCLUDEDIR} "
#CMAKE_ARGUMENTS+="-DBoost_LIBRARIES=${BOOST_LIBRARIES} "

# Set JGTL
# Use the JGTL variable above to set JGTL include dir
#CMAKE_ARGUMENTS+="-DJGTL_INCLUDE=${JGTL_include} "

# Set QT
# Use the QT variables above to set QT libraries
#CMAKE_ARGUMENTS+="-DQT_INCLUDE=${QT_INCLUDEDIRS} "
#CMAKE_ARGUMENTS+="-DQT_LIB=${QT_LIBRARYDIR} "
#CMAKE_ARGUMENTS+="-DQT_GUI=${QT_GUI} "
#CMAKE_ARGUMENTS+="-DQT_CORE=${QT_CORE} "


# Build cppn_examiner
echo Building CPPN Examiner
mkdir -p ${buildDir}
cd ${buildDir}
echo qmake -spec macx-g++ ${rootDir}/cppn-x.pro
qmake -spec macx-g++ ${rootDir}/cppn-x.pro
make
cd ${rootDir}

#cd ${buildDir}
#echo cmake ${rootDir} ${CMAKE_ARGUMENTS}
#cmake ${rootDir} ${CMAKE_ARGUMENTS}
#make
#cd ${rootDir}
#echo Finished building CPPN Examiner
#echo

