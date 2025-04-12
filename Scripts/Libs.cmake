if(PNG STREQUAL SYSTEM)
	find_package(PNG REQUIRED)

	list(APPEND Libs ${PNG_LIBRARIES})
	
	include_directories(${PNG_INCLUDE_DIRS})
endif()

if(FREETYPE STREQUAL SYSTEM)
	find_package(Freetype REQUIRED)

	list(APPEND Libs ${FREETYPE_LIBRARIES})   
	
	include_directories(${FREETYPE_INCLUDE_DIRS})
elseif(FREETYPE STREQUAL "BUILTIN")
	set(FREETYPE_INCLUDE_DIRS  "${CMAKE_SOURCE_DIR}/Source/External/freetype/include")
endif()

if(FLAC STREQUAL "SYSTEM")
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindFLAC.cmake)

    list(APPEND Libs ${FLAC_LIBRARIES})
    
    include_directories(${FLAC_INCLUDE_DIRS})
endif()

if(VORBIS STREQUAL "SYSTEM")
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindVorbis.cmake)
    
    list(APPEND Libs ${VORBIS_LIBRARIES})
    
    include_directories(${VORBIS_INCLUDE_DIRS})
endif()


############# BUILT-IN Libraries #################3


if(PNG STREQUAL "BUILTIN")
	add_subdirectory(Source/External/zlib)
	include_directories(Source/External/zlib/include)

	add_subdirectory(Source/External/png)
	include_directories(Source/External/png/include)
	target_link_libraries(Gorgon png)

	target_link_libraries(png zlib)
endif()

if(FLAC STREQUAL "BUILTIN")
	add_subdirectory(Source/External/flac)

	include_directories(Source/External/flac/include)

	add_definitions("-DFLAC__NO_DLL")

	target_link_libraries(Gorgon flac)
endif()

if(FREETYPE STREQUAL "BUILTIN")
	add_subdirectory(Source/External/freetype)

	include_directories(Source/External/freetype/include)
	set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Source/External/freetype/include)

	target_link_libraries(Gorgon freetype)
endif() 

if(VORBIS STREQUAL "BUILTIN")
	add_subdirectory(Source/External/vorbis)

	include_directories(Source/External/vorbis/include)

	target_link_libraries(Gorgon vorbis)

	add_subdirectory(Source/External/ogg)

	include_directories(Source/External/ogg/include)
endif()

if(FONTCONFIG STREQUAL "BUILTIN")
	message(FATAL_ERROR "Built-in fontconfig is not supported yet.")
endif()
