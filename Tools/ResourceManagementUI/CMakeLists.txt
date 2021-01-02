cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0054 NEW)

set(PROJECT_NAME "ResourceManagementUI")

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Bin)

find_package(Gorgon REQUIRED)

add_compile_options(-std=c++14)

include(../../Scripts/Macros.cmake)

set(DebugLibs "")
set(OptimizedLibs "")

StartSource(Source)

add_executable(${PROJECT_NAME} WIN32
    ${All}
)

linkgorgon(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Resource/GRM-Logo-72x72.png ${CMAKE_CURRENT_SOURCE_DIR}/Bin/GRM-Logo-72x72.png COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Resource/Font/Goldman-Regular.ttf ${CMAKE_CURRENT_SOURCE_DIR}/Bin/Goldman-Regular.ttf COPYONLY)
