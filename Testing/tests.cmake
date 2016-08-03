CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
	Filesystem
	Gscript
	Generic
	PDParser
	Window
)

SET(UnitTests
	Enum
	Event
	Filesystem
	FLAC
	GarbageCollection
	Geometry
	Hashmap
	String
	Logging
	URI
)
IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()
