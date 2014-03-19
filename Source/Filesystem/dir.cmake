cmake_minimum_required(VERSION 2.8)

SET(Local
	${wd}/Filesystem.cpp
	${wd}/Iterator.h
	"${wd}/../Filesystem.h"
)

IF(${WIN32})
	LIST(APPEND Local
		${wd}/Windows.cpp
	)
ELSE()
	LIST(APPEND Local
		${wd}/Linux.cpp
	)
ENDIF()

SET(Sub
)

DoSource()
