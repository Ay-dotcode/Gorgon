set(PROJECT_NAME "Map Loader")

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Bin)

find_package(Gorgon REQUIRED)

add_compile_options(-std=c++20)

add_executable(${PROJECT_NAME}
        Source/main.cpp
        Source/MapLoader/MapLoader.cpp
        Source/Matrix/Impl/FMatrix.cpp
        Source/Matrix/Impl/IMatrix.cpp
        Source/MapLoader/MapLoaderTestUI.cpp
        Source/matrix.test.h
)

linkgorgon(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
