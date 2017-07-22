cmake_minimum_required(VERSION 2.8)

OPTION(IGNORE_COMPILER_VERSION "Ignore compiler version check" OFF)
OPTION(IGNORE_COMPILER_VENDOR "Ignore compiler vendor check" OFF)

set(CMAKE_CXX_STANDARD 14)

#compiler check
IF(CMAKE_COMPILER_IS_GNUCXX)
	EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	
	IF(GCC_VERSION VERSION_LESS 4.8)
		IF(IGNORE_COMPILER_VERSION)
			MESSAGE(STATUS "Gorgon Library requires GCC 4.8+")
		ELSE()
			MESSAGE(STATUS "use cmake -DIGNORE_COMPILER_VERSION=ON to continue anyway")
			MESSAGE(FATAL_ERROR "Gorgon Library requires GCC 4.8+")
		ENDIF()
	ENDIF()
ELSEIF(MSVC)
	IF(MSVC_VERSION VERSION_LESS 14)
		IF(IGNORE_COMPILER_VERSION)
			MESSAGE(STATUS "Gorgon Library requires Visual Studio 2015+")
		ELSE()
			MESSAGE(STATUS "use cmake -DIGNORE_COMPILER_VERSION=ON to continue anyway")
			MESSAGE(FATAL_ERROR "Gorgon Library requires Visual Studio 2015+")
		ENDIF()
	ENDIF()
ELSE()
	IF(IGNORE_COMPILER_VENDOR)
		MESSAGE(STATUS "Unsupported compiler! Gorgon Library requires GCC 4.8+ OR Visual Studio 2015+")
		MESSAGE(STATUS "You may need to manually activate C++14 features")
	ELSE()
		MESSAGE(STATUS "use cmake -DIGNORE_COMPILER_VENDOR=ON to continue anyway")
		MESSAGE(FATAL_ERROR "Unsupported compiler! Gorgon Library requires GCC 4.8+ OR Visual Studio 2015+")
	ENDIF()
ENDIF()

OPTION(ENABLE_64_BIT "Enable 64 bit compilation" OFF)

#enable C++14 and 32-bit compilation
INCLUDE(CheckCXXCompilerFlag)
IF(CMAKE_COMPILER_IS_GNUCXX)
	IF(${ENABLE_64_BIT})
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	ELSE()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -m32")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	ENDIF()
ENDIF()

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	IF(${ENABLE_64_BIT})
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	ELSE()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -m32")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	ENDIF()
ENDIF()

#MACRO(FixProject) 
#ENDMACRO()

#make sure 64bits cannot be activated
IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
	
	ADD_DEFINITIONS(-DUNICODE)
	ADD_DEFINITIONS(-D_UNICODE)
	
	SET(configs
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
	
	
	add_definitions( "/D_CRT_SECURE_NO_WARNINGS /wd4068" )
	
	FOREACH(config ${configs})
      IF(${config} MATCHES "/MD")
        STRING(REGEX REPLACE "/MD" "/MT" ${config} "${${config}}")
      ENDIF()
    ENDFOREACH()
	
	IF(${CMAKE_CL_64} AND NOT(${ENABLE_64_BIT}))
		MESSAGE(FATAL_ERROR "Gorgon Library works only 32bits")
	ENDIF()
ENDIF()
