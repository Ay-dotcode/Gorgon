SET(Local
	../Time.h
	Time.cpp
	Performance.h
)

IF(${WIN32})
	LIST(APPEND Local
		Windows.cpp
	)
ELSE()
	LIST(APPEND Local
		Linux.cpp
	)
ENDIF()
