CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
    Animation
	Clipboard
	DnD
	Filesystem
	Gscript
	Generic
	Audio
	PDParser
	Scene
	Window
	Font
	HTMLRenderer
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
	Scene
	ScopeGuard
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
