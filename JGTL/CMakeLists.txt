Project(JGTL)

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

IF(NOT MYPROJ_RAN_ONCE)
   SET(MYPROJ_RAN_ONCE 1 CACHE INTERNAL "")

	SET(
	CMAKE_INSTALL_PREFIX
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	FILEPATH
	"Install path prefix, prepended onto install directories."
	FORCE
	)

	SET(
	EXECUTABLE_OUTPUT_PATH
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	FILEPATH
	"Single output directory for building all executables."
	FORCE
	)

	SET(
	LIBRARY_OUTPUT_PATH
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	FILEPATH
	"Single output directory for building all libraries."
	FORCE
	)

	SET(
	CMAKE_BUILD_TYPE
	"Release"
	CACHE
	STRING
	"Description: Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
	FORCE
	)

ENDIF(NOT MYPROJ_RAN_ONCE)

INCLUDE_DIRECTORIES(
	include
)

ADD_EXECUTABLE(
	CircularBufferExample

	examples/CircularBuffer.cpp
)

SET_TARGET_PROPERTIES(CircularBufferExample PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	PoolMapExample

	examples/PoolMap.cpp
)

SET_TARGET_PROPERTIES(PoolMapExample PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	VariantExample

	examples/Variant.cpp
)

SET_TARGET_PROPERTIES(VariantExample PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	QuickProfExample

	examples/QuickProf.cpp
)

SET_TARGET_PROPERTIES(QuickProfExample PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	UnitsExample

	examples/Units.cpp
)

SET_TARGET_PROPERTIES(UnitsExample PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	MapTests

	tests/MapTests.cpp
)

SET_TARGET_PROPERTIES(MapTests PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
	VariantTests

	tests/VariantTests.cpp
)

SET_TARGET_PROPERTIES(VariantTests PROPERTIES DEBUG_POSTFIX _d)


