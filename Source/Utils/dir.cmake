SET(Local
	Assert.h
)


IF(${WIN32})
	LIST(APPEND Local
		#Assert_Win32.cpp
	)
ELSE()
	LIST(APPEND Local
		Assert_Linux.cpp
	)
ENDIF()
