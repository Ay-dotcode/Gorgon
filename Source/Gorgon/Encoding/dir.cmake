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

IF(VORBIS STREQUAL "BUILTIN" OR VORBIS STREQUAL "SYSTEM")
    list(APPEND Local 
        Vorbis.h 
        Vorbis.cpp
    )
ENDIF()
