CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(ManualTests
    Animation
	Clipboard
	CGI
	DnD
	Filesystem
	FreeType
	Gscript
	Generic
	Audio
	PDParser
	Scene
	Window
	Font
	HTMLRenderer
	UI_Component
	UI_Generate
)

SET(UnitTests
	Enum
	Event
	Filesystem
	FLAC
	GarbageCollection
	Geometry
	Hashmap
	Layer
	Logging
	EncodingImage
	Property
	Scene
	ScopeGuard
	String
	URI
)


option(UNIT_TESTS "Enable all unit tests." OFF)

IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()
