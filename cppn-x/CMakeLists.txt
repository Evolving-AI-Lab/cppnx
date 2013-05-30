cmake_minimum_required(VERSION 2.6)
project(cppn-x)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

SET(cppn-x_SRCS
	src/CE_Node.cpp
	src/CE_Edge.cpp
	src/CE_Window.cpp
	src/CE_CppnWidget.cpp
)

SET(cppn-x_MOC_HDRS
	include/CE_CppnWidget.h
	include/CE_Window.h
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

SET(
	NEAT_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../NEAT/include"
	CACHE
	FILEPATH 
	"NEAT include directory"
) 

SET(
	TINYXMLDLL_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../tinyxmldll/include"
	CACHE
	FILEPATH 
	"TinyXMLDLL include directory"
) 

SET(
	QT_INCLUDE
	"/Library/Frameworks/QtCore.framework/Versions/4/Headers"
	"/Library/Frameworks/QtGui.framework/Versions/4/Headers"
	CACHE
	FILEPATH 
	"QT include directories"
)
#Adding /usr/local/Qt4.8/mkspecs/macx-g++ if things fail might work

if(Boost_LIBRARIES)
	message("Boost_LIBRARIES defined in compile.sh script")
else(Boost_LIBRARIES)
	message("Boost_LIBRARIES NOT defined. Running find_package")
	unset(Boost_INCLUDE_DIRS CACHE)
	unset(Boost_LIBRARY_DIRS CACHE)
	unset(Boost_LIBRARIES CACHE)
	find_package( Boost 1.34 COMPONENTS filesystem system thread regex)
endif(Boost_LIBRARIES)

if(QT_GUI)
	message("QT_GUI defined in compile.sh script")
else(QT_GUI)
	message("QT_GUI NOT defined. Running find_library")
	unset(QT_GUI CACHE)
	FIND_LIBRARY(QT_GUI QtGui)
endif(QT_GUI)

if(QT_CORE)
	message("QT_CORE defined in compile.sh script")
else(QT_CORE)
	message("QT_CORE NOT defined. Running find_library")
	unset(QT_CORE CACHE)
	FIND_LIBRARY(QT_CORE QtCore)
endif(QT_CORE)

#Print libraries found
message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 
message("Boost_LIBRARIES   : ${Boost_LIBRARIES}") 
message("QT_GUI : ${QT_GUI}") 
message("QT_CORE: ${QT_CORE}") 




#Set include directories:
INCLUDE_DIRECTORIES(
	include
	${NEAT_INCLUDE}
	${JGTL_INCLUDE}
	${TINYXMLDLL_INCLUDE}
	${Boost_INCLUDE_DIRS}
	${QT_INCLUDE}
)

#Set link directories:
LINK_DIRECTORIES(
	${NEAT_LIB}
	${TINYXMLDLL_LIB}
	${Boost_LIBRARY_DIRS}
	${QT_LIB}
)

#Set target:
ADD_EXECUTABLE(
	cppn-x
	src/CE_Main.cpp
	${cppn-x_SRCS}
	${cppn-x_MOC_SRCS}
)

ADD_DEPENDENCIES(cppn-x NEAT)

#Set libraries:
TARGET_LINK_LIBRARIES(
	cppn-x
	${Boost_LIBRARIES}
	${QT_GUI}
	${QT_CORE}

	debug
	tinyxmldll_d

	optimized
	tinyxmldll

	debug
	NEAT_d

	optimized
	NEAT		
)


