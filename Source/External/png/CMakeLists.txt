cmake_minimum_required(VERSION 3.25)

set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 6)
set(PNGLIB_REVISION 47)
set(PNGLIB_SUBREVISION 0)
#set(PNGLIB_SUBREVISION "git")
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_REVISION})
set(PNGLIB_ABI_VERSION ${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_SHARED_VERSION ${PNGLIB_ABI_VERSION}.${PNGLIB_REVISION}.${PNGLIB_SUBREVISION})

include_directories(include)
include_directories(src)

set(Local
    src/png.c
    src/pngerror.c
    src/pngget.c
    src/pngmem.c
    src/pngpread.c
    src/pngread.c
    src/pngrio.c
    src/pngrtran.c
    src/pngrutil.c
    src/pngset.c
    src/pngtrans.c
    src/pngwio.c
    src/pngwrite.c
    src/pngwtran.c
    src/pngwutil.c

    src/pngpriv.h
    src/pngdebug.h
    src/pnginfo.h
    src/pngstruct.h

    src/png.h
    src/pngconf.h
)

add_library(png STATIC
    ${Local}
)

IF(WIN32)
    set_target_properties(png PROPERTIES
        DEBUG_OUTPUT_NAME png_d
        RELWITHDEBINFO_OUTPUT_NAME  png_rd
        MINSIZEREL_OUTPUT_NAME  png_min
    )
ENDIF()

if(MSVC OR (WIN32 AND (CMAKE_C_COMPILER_ID MATCHES "Clang")))
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

install(TARGETS png
    EXPORT GorgonTargets
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/Gorgon"
)

export(TARGETS png APPEND FILE "${PROJECT_PACKAGE_DIR}/GorgonTargets.cmake")
