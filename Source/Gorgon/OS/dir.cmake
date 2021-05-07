SET(Local
	../OS.h
	OS.cpp
)

IF(${WIN32})
	LIST(APPEND Local
		Win32.cpp
	)
	
	LIST(APPEND Libs Secur32)
	
	LIST(APPEND Libs Netapi32)
ELSE()
	LIST(APPEND Local
		Linux.cpp
	)	
ENDIF()

IF(FONTCONFIG STREQUAL "SYSTEM")
    find_package(Fontconfig)
    
    include_directories(${Fontconfig_INCLUDE_DIRS})
    LIST(APPEND Libs ${Fontconfig_LIBRARIES})
ENDIF()
