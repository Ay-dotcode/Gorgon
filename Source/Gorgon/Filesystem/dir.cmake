SET(Local
	Filesystem.cpp
	Iterator.h
	../Filesystem.h
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
