CMAKE_MINIMUM_REQUIRED(VERSION 3.25)

SET(ManualTests
	AdvancedText
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
	UI_WidgetTest
)

SET(UnitTests
	Enum
	Event
	Filesystem
	FLAC
	GarbageCollection
	Geometry
	Path
	Hashmap
	KeyRepeater
	Layer
	Logging
	EncodingImage
	Property
	Scene
	ScopeGuard
	String
	URI
	IO
)


option(UNIT_TESTS "Enable all unit tests." OFF)

IF(${SCRIPTING})
	LIST(APPEND UnitTests
		Scripting
	)
ENDIF()
