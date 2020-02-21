SET(Local
	../WindowManager.h
	WindowManager.cpp
)

IF(${WIN32})
	LIST(APPEND Local
		DWM.cpp
	)
ELSE()
	LIST(APPEND Local
		X11
	)
	
	
ENDIF()

