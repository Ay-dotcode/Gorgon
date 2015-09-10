SET(Local
	Assert.h
)


IF(${WIN32})
	LIST(APPEND Local
		Assert_Win32.cpp
	)
ELSE()
	LIST(APPEND Local
		Assert_Linux.cpp
	)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
	LIST(APPEND Local
		Compiler_GCC.cpp
	)
ELSEIF(MSVC)
	LIST(APPEND Local
		Compiler_MSVC.cpp
	)
ENDIF()
