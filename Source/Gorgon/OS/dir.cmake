SET(Local
	../OS.h
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

