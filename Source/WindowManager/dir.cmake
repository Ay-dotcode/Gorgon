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
		X11.cpp
	)
	
	#INCLUDE(${CMAKE_CURRENT_LIST_DIR}/XRANDR.cmake)
	
	FIND_PACKAGE(X11 REQUIRED)
	#FIND_PACKAGE(Xrandr REQUIRED)

	LIST(APPEND Libs ${X11_LIBRARIES}  ${X11_Xrandr_LIB} ${X11_Xinerama_LIB})

ENDIF()

