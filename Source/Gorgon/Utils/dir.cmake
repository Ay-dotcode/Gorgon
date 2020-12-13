SET(Local
    Console.h
	Assert.h
	Logging.h
	RefCounter.h
	ScopeGuard.h
)



IF(${WIN32})
	LIST(APPEND Local
		Assert_Win32.cpp
		Console_Win32.cpp
	)
ELSE()
	LIST(APPEND Local
		Assert_Linux.cpp
		Console_Nix.cpp
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
