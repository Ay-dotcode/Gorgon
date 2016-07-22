cmake_minimum_required(VERSION 2.8.3)

project(Gorgon)

set(GORGON_MAJOR_VERSION 4)
set(GORGON_MINOR_VERSION 0)
set(GORGON_PATCH_VERSION 0)
set(GORGON_VERSION_ALPHA 1)
set(GORGON_VERSION ${GORGON_MAJOR_VERSION}.${GORGON_MINOR_VERSION}.${GORGON_PATCH_VERSION})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Bin)
set(CMAKE_TESTING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Testing)
set(CMAKE_DOCUMENT_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Docs)
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/Bin)
set(PROJECT_PACKAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/Package)

#### SETTINGS ####

option(OPENGL "Use OpenGL as underlying GL. Should be ON for now." ON)

option(AUDIO "Enable audio." ON)

option(GORGON_FAST_ANY "Make any to work fast by removing empty checks. This will only apply to release mode." OFF)

option(SCRIPTING "Enable scripting module." ON)

option(NO_LOGGING "Disables logger globally. If set, unit tests for logging will fail." OFF)

option(TESTMODE "Enable test mode." OFF)

option(DOCUMENTATION_GRAPHS "Enable graphs in documentation." OFF)

option(UNIT_TESTS "Enable compiling of unit tests." OFF)

option(FLAC_SUPPORT "Enable FLAC decoding and streaming." ON)

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")

set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")

set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

######################


if(AUDIO AND (NOT AUDIOLIB OR AUDIOLIB STREQUAL ""))
	if(WIN32)
		set(AUDIOLIB WASAPI)
	else()
		set(AUDIOLIB PULSE)
	endif()
endif()

if(AUDIO)
	add_definitions("-DAUDIO")
	add_definitions("-DAUDIO_${AUDIOLIB}")
endif()

if(SCRIPTING)
	add_definitions("-DSCRIPTING")
endif()

if(DOCUMENTATION_GRAPHS)
	set(DOXYGRAPH YES)
else()
	set(DOXYGRAPH NO)
endif()

if(TESTMODE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEST")
endif()

if(NO_LOGGING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_LOGGING")
endif()

if(GORGON_FAST_ANY)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DGORGON_FAST_ANY")
endif()

if(OPENGL)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENGL")
endif()

if(UNIX) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -Werror=return-type")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_SWIZZLE -DGLM_FORCE_RADIANS")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

include(Scripts/Compiler.cmake)

include(Scripts/Macros.cmake)

set(DebugLibs "")
set(OptimizedLibs "")

StartSource(Source/Gorgon)

if(UNIT_TESTS)
	include(Scripts/Testing.cmake)
endif()

include(Scripts/Doxygen.cmake)

add_library(Gorgon STATIC ${All})

include_directories(Source/External)
include_directories(Source)

target_link_libraries(Gorgon ${Libs})

if(NOT DebugLibs STREQUAL "")
	target_link_libraries(Gorgon debug ${DebugLibs})
endif()

if(NOT OptimizedLibs STREQUAL "")
	target_link_libraries(Gorgon optimized ${OptimizedLibs})
endif()


install(TARGETS Gorgon
	EXPORT GorgonTargets
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)


export(TARGETS Gorgon FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
export(PACKAGE Gorgon)

if(${FLAC_SUPPORT}) 
	add_subdirectory(Source/External/flac)
	
	include_directories(Source/External/flac/include)

	add_definitions("-DFLAC__NO_DLL")
	
	target_link_libraries(Gorgon flac)
endif()

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")

set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Source")

configure_file(Scripts/GorgonConfig.cmake.in
  "${PROJECT_PACKAGE_DIR}/GorgonConfig.cmake" @ONLY)
  
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Source")
configure_file(Scripts/GorgonConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GorgonConfig.cmake" @ONLY)

configure_file(Scripts/GorgonConfigVersion.cmake.in
  "${PROJECT_PACKAGE_DIR}/GorgonConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GorgonConfig.cmake"
  "${PROJECT_PACKAGE_DIR}/GorgonConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(EXPORT GorgonTargets DESTINATION "${INSTALL_CMAKE_DIR}")
