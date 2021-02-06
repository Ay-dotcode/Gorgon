SET(Local
	LZMA.h
	LZMA.cpp
	PNG.h
	PNG.cpp
	URI.h
	URI.cpp
	JPEG.h
	JPEG.cpp
)

IF(FLAC STREQUAL "BUILTIN")
	list(APPEND Local 
		FLAC.h 
		FLAC.cpp
	)
ENDIF()

if(FLAC STREQUAL "SYSTEM")
	list(APPEND Local 
		FLAC.h 
		FLAC.cpp
	)
	
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindFLAC.cmake)

	list(APPEND Libs ${FLAC_LIBRARIES})
	
	include_directories(${FLAC_INCLUDE_DIRS})
endif()
