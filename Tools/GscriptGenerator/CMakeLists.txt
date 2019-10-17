cmake_minimum_required(VERSION 2.8)

project(GscriptGenerator)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/Bin)

if(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

if(MSVC)
	ADD_DEFINITIONS(-D_SBCS -D_CRT_SECURE_NO_WARNINGS)
	
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
endif()

add_executable(GscriptGenerator Source/GscriptGenerator.cpp)

