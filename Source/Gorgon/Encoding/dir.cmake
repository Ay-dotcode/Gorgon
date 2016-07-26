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

IF(${FLAC_SUPPORT})
	list(APPEND Local 
		FLAC.h 
		FLAC.cpp
	)
ENDIF()
