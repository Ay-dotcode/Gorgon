SET(Local
	../Time.h
	Time.cpp
	Timer.h
)

IF(${WIN32})
	LIST(APPEND Local
		Windows.cpp
	)
	
	LIST(APPEND Libs "winmm.lib")
ELSE()
	LIST(APPEND Local
		Linux.cpp
	)
ENDIF()
