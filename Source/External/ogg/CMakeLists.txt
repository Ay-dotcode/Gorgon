cmake_minimum_required(VERSION 3.5)

include_directories(include)


SET(Local 
    src/bitwise.c
    src/framing.c
)

add_library(ogg STATIC 
	${Local}
)

IF(WIN32)
	set_target_properties(ogg PROPERTIES 
		DEBUG_OUTPUT_NAME ogg_d
		RELWITHDEBINFO_OUTPUT_NAME  ogg_rd
		MINSIZEREL_OUTPUT_NAME  ogg_min
	)
ENDIF()

install(TARGETS ogg
	EXPORT GorgonTargets
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)

export(TARGETS ogg APPEND FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
