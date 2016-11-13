CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
	Filesystem
	Gscript
	Audio
	PDParser
	Window
	Font
)

SET(UnitTests
	Enum
	Event
	Filesystem
	FLAC
	GarbageCollection
	Geometry
	Hashmap
	EncodingImage
	String
	Logging
	URI
)
IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()
