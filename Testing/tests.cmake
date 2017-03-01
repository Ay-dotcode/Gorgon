CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
    Animation
	Filesystem
	Gscript
	Generic
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


option(UNIT_TESTS "Enable all unit tests." OFF)

IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()
