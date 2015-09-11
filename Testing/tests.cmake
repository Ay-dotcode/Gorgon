CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
	Filesystem
	Gscript
	PDParser
)

SET(UnitTests
	Enum
	Event
	Filesystem
	GarbageCollection
	Geometry
	Hashmap
	String
)
IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()