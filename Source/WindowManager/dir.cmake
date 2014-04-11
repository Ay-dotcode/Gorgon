SET(Local
	../WindowManager.h
)

IF(${WIN32})
	LIST(APPEND Local
		Windows.cpp
	)
ELSE()
	LIST(APPEND Local
		X11.cpp
	)	
	
	FIND_PACKAGE(X11 REQUIRED)

	LIST(APPEND Libs ${X11_LIBRARIES})

ENDIF()

