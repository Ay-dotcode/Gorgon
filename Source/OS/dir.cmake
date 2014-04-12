SET(Local
	../OS.h
)

IF(${WIN32})
	LIST(APPEND Local
		Win32.cpp
	)
ELSE()
	LIST(APPEND Local
		Linux.cpp
	)	
ENDIF()

