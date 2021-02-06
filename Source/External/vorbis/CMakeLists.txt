cmake_minimum_required(VERSION 3.5)

include_directories(include)
include_directories(src)

set(Local
    src/mdct.c
    src/smallft.c
    src/block.c
    src/envelope.c
    src/window.c
    src/lsp.c
    src/lpc.c
    src/analysis.c
    src/synthesis.c
    src/psy.c
    src/info.c
    src/floor1.c
    src/floor0.c
    src/res0.c
    src/mapping0.c
    src/registry.c
    src/codebook.c
    src/sharedbook.c
    src/lookup.c
    src/bitrate.c
    src/vorbisfile.c
    src/vorbisenc.c
)

add_library(vorbis STATIC 
	${Local}
)

target_link_libraries(vorbis ogg)
include_directories("${CMAKE_CURRENT_LIST_DIR}/../ogg/include")

IF(WIN32)
	set_target_properties(vorbis PROPERTIES 
		DEBUG_OUTPUT_NAME vorbis_d
		RELWITHDEBINFO_OUTPUT_NAME  vorbis_rd
		MINSIZEREL_OUTPUT_NAME  vorbis_min
	)
ENDIF()

install(TARGETS vorbis
	EXPORT GorgonTargets
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)

export(TARGETS vorbis APPEND FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
