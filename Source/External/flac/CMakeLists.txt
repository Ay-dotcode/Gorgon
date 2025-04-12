cmake_minimum_required(VERSION 3.25)

include_directories(include)
include_directories(src/include)

add_definitions("-DFLAC__NO_DLL")

SET(Local 
    src/bitmath.c
    src/bitreader.c
    src/bitwriter.c
    src/cpu.c
    src/crc.c
    src/fixed.c
    src/float.c
    src/format.c
    src/lpc.c
    src/md5.c
    src/memory.c
    src/metadata_iterators.c
    src/metadata_object.c
    src/stream_decoder.c
    src/stream_encoder.c
    src/stream_encoder_framing.c
    src/window.c
    $<$<BOOL:${WIN32}>:src/windows_unicode_filenames.c>
)

add_library(flac STATIC 
	${Local}
)

IF(WIN32)
	set_target_properties(flac PROPERTIES 
		DEBUG_OUTPUT_NAME flac_d
		RELWITHDEBINFO_OUTPUT_NAME  flac_rd
		MINSIZEREL_OUTPUT_NAME  flac_min
	)
ENDIF()

install(TARGETS flac
	EXPORT GorgonTargets
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)

export(TARGETS flac APPEND FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
