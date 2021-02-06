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

IF(FLAC STREQUAL "BUILTIN" OR FLAC STREQUAL "SYSTEM")
    list(APPEND Local 
        FLAC.h 
        FLAC.cpp
    )
ENDIF()

if(FLAC STREQUAL "SYSTEM")
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindFLAC.cmake)

    list(APPEND Libs ${FLAC_LIBRARIES})
    
    include_directories(${FLAC_INCLUDE_DIRS})
endif()

IF(VORBIS STREQUAL "BUILTIN" OR VORBIS STREQUAL "SYSTEM")
    list(APPEND Local 
        Vorbis.h 
        Vorbis.cpp
    )
ENDIF()

if(VORBIS STREQUAL "SYSTEM")
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindVorbis.cmake)
    
    list(APPEND Libs ${VORBIS_LIBRARIES})
    
    include_directories(${VORBIS_INCLUDE_DIRS})
endif()
