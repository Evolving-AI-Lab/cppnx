cmake_minimum_required(VERSION 2.6)
Project(NEAT)

IF(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc")

SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc")
SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc")

ELSE(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} -pg")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_DEBUG} -pg")

ENDIF(MSVC)

ADD_DEFINITIONS(-DBUILD_NEAT_DLL -DTIXML_USE_STL)

INCLUDE_REGULAR_EXPRESSION( "^.*[Nn][Ee][Aa][Tt][_].*$" )

#Set default directories:
SET(
	TINYXMLDLL_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../tinyxmldll/include"
	CACHE
	FILEPATH 
	"TinyXMLDLL include directory"
) 

SET(
	JGTL_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../JGTL/include"
	CACHE
	FILEPATH 
	"JGTL include directory"
) 

SET(
	ZLIB_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../zlib"
	CACHE
	FILEPATH 
	"Zlib include directory"
) 

#Set includes directories:
INCLUDE_DIRECTORIES(
	include
	${TINYXMLDLL_INCLUDE}
	${Boost_INCLUDE_DIRS}
	${JGTL_INCLUDE}
	${ZLIB_INCLUDE}
)

#Set link directories
LINK_DIRECTORIES(
	${TINYXMLDLL_LIB}
	${BOOST_STAGE}
)

#Set target:
ADD_LIBRARY(
NEAT
SHARED 

src/NEAT_FastNetwork.cpp
src/NEAT_FastBiasNetwork.cpp
src/NEAT_GeneticGene.cpp
src/NEAT_GeneticGeneration.cpp
src/NEAT_GeneticIndividual.cpp
src/NEAT_GeneticLinkGene.cpp
src/NEAT_GeneticNodeGene.cpp
src/NEAT_GeneticPopulation.cpp
src/NEAT_GeneticSpecies.cpp
src/NEAT_Globals.cpp
src/NEAT_ModularNetwork.cpp
src/NEAT_Network.cpp
src/NEAT_NetworkLink.cpp
src/NEAT_NetworkNode.cpp
src/NEAT_Random.cpp
src/cRandom.cc

include/NEAT_FastNetwork.h
include/NEAT_FastBiasNetwork.h
include/NEAT_GeneticGene.h
include/NEAT_GeneticGeneration.h
include/NEAT_GeneticIndividual.h
include/NEAT_GeneticLinkGene.h
include/NEAT_GeneticNodeGene.h
include/NEAT_GeneticPopulation.h
include/NEAT_GeneticSpecies.h
include/NEAT_Globals.h
include/NEAT.h
include/NEAT_Defines.h
include/NEAT_ModularNetwork.h
include/NEAT_Network.h
include/NEAT_NetworkLink.h
include/NEAT_NetworkNode.h
include/NEAT_Random.h
include/NEAT_STL.h
include/cRandom.h
)

#Set libraries
TARGET_LINK_LIBRARIES(
NEAT 

debug
tinyxmldll_d

optimized
tinyxmldll

boost_filesystem
boost_system

)

SET_TARGET_PROPERTIES(NEAT PROPERTIES DEBUG_POSTFIX _d)
