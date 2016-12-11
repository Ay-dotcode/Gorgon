SET(Local
	Animation
	Containers
	Encoding
	External
	Filesystem
	Geometry
	GL
	Graphics
	Input
	IO
	Network
	OS
	Resource
	String
	Time
	WindowManager
	Any.h
	Enum.h
	Event.h
	Layer.h
	Layer.cpp
	Main.h
	Main.cpp
	SGuid.h
	SGuid.cpp
	Struct.h
	Threading.h
	TMP.h
	Types.h
	Utils
	Window.h
	Window.cpp
)

IF(${AUDIO})
	LIST(APPEND Local
		Audio
	)
ELSEIF()
	LIST(APPEND Local
		Audio/Basic.h
	)
ENDIF()

IF(${SCRIPTING})
	LIST(APPEND Local
		Scripting
	)
ENDIF()

