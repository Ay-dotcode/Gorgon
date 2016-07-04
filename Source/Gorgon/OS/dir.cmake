SET(Local
	../OS.h
)

IF(${WIN32})
	LIST(APPEND Local
		Win32.cpp
	)
	
	FIND_LIBRARY(Secur32.lib REQUIRED)
	LIST(APPEND Libs Secur32)
	
	FIND_LIBRARY(Netapi32.lib REQUIRED)
	LIST(APPEND Libs Netapi32)
ELSE()
	LIST(APPEND Local
		Linux.cpp
	)	
ENDIF()

