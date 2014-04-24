SET(Local
	../WindowManager.h
	WindowManager.cpp
)

FIND_PACKAGE(OpenGL REQUIRED)

LIST(APPEND Libs ${OPENGL_LIBRARIES})

IF(${WIN32})
	LIST(APPEND Local
		DWM.cpp
	)
ELSE()
	LIST(APPEND Local
		X11.cpp
	)
	
	FIND_PACKAGE(X11 REQUIRED)

	LIST(APPEND Libs ${X11_LIBRARIES})

ENDIF()

