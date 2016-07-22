cmake_minimum_required(VERSION 2.8)

include_directories(include)

add_definitions("-DFLAC__NO_DLL")

add_library(flac STATIC 
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
	src/win_utf8_io.c
)


install(TARGETS flac
	EXPORT GorgonTargets
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)

export(TARGETS flac APPEND FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
