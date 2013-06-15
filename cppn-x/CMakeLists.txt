cmake_minimum_required(VERSION 2.6)
project(cppn-x)

set(CMAKE_PREFIX_PATH C:\\Qt\\4.8.4)
find_package(Qt4 REQUIRED QtCore QtGui)

include(${QT_USE_FILE})

SET(cppn-x_SRCS
	src/CE_Node.cpp
	src/CE_Edge.cpp
	src/CE_Window.cpp
	src/CE_CppnWidget.cpp
	src/CE_Cppn.cpp
	src/CE_CppnParser.cpp
	src/CE_ActivationFunctions.cpp
	src/CE_CppnWriter.cpp
	src/CE_ColorButton.cpp
	src/CE_NodeView.cpp
	src/CE_FinalNodeView.cpp
)

SET(cppn-x_MOC_HDRS
	include/CE_CppnWidget.h
	include/CE_Window.h
	include/CE_ColorButton.h
)



QT4_WRAP_CPP(cppn-x_MOC_SRCS ${cppn-x_MOC_HDRS})

ADD_DEFINITIONS(
	-DTIXML_USE_STL
	-DNOPCH
	-D_USE_MATH_DEFINES
	-pthread
	-DONETHREAD
	-DQT_GUI_LIB 
	-DQT_CORE_LIB 
	-DQT_SHARED
)

INCLUDE_REGULAR_EXPRESSION( "^.*([Hh][Cc][Uu][Bb][Ee]|[Nn][Ee][Aa][Tt]|main).*$" )

#Set default locations:
SET(
	JGTL_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../JGTL/include"
	CACHE
	FILEPATH 
	"JGTL include directory"
)

#SET(
#	QT_INCLUDE
#	"/Library/Frameworks/QtCore.framework/Versions/4/Headers"
#	"/Library/Frameworks/QtGui.framework/Versions/4/Headers"
#	CACHE
#	FILEPATH 
#	"QT include directories"
#)
#Adding /usr/local/Qt4.8/mkspecs/macx-g++ if things fail might work

message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 

if(Boost_INCLUDE_DIRS)
	message("Boost_INCLUDE_DIRS defined in compile.sh script")
else(Boost_INCLUDE_DIRS)
	message("Boost_INCLUDE_DIRS NOT defined. Running find_package")
	unset(Boost_INCLUDE_DIRS CACHE)
	find_package( Boost 1.34 )
endif(Boost_INCLUDE_DIRS)

#if(QT_GUI)
#	message("QT_GUI defined in compile.sh script")
#else(QT_GUI)
#	message("QT_GUI NOT defined. Running find_library")
#	unset(QT_GUI CACHE)
#	FIND_LIBRARY(QT_GUI QtGui)
#endif(QT_GUI)

#if(QT_CORE)
#	message("QT_CORE defined in compile.sh script")
#else(QT_CORE)
#	message("QT_CORE NOT defined. Running find_library")
#	unset(QT_CORE CACHE)
#	FIND_LIBRARY(QT_CORE QtCore)
#endif(QT_CORE)


#if ( NOT Qt_FOUND OR NOT QtCore_FOUND )
#  message(FATAL_ERROR "Package Qt and component QtXml required, but not found!")
#endif( NOT Qt_FOUND OR NOT QtCore_FOUND )

#Print libraries found
message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
message("QT_LIBRARIES : ${QT_LIBRARIES}")
message("QT_INCLUDES : ${QT_INCLUDES}")



#Set include directories:
INCLUDE_DIRECTORIES(
	include
	${JGTL_INCLUDE}
	${Boost_INCLUDE_DIRS}
	${QT_INCLUDES}
)

#Set link directories:
LINK_DIRECTORIES(
	${QT_LIB}
)

#Set target:
ADD_EXECUTABLE(
	cppn-x
	src/CE_Main.cpp
	${cppn-x_SRCS}
	${cppn-x_MOC_SRCS}
)

#Set libraries:
TARGET_LINK_LIBRARIES(
	cppn-x
	${QT_LIBRARIES}	
)


