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
	Utils
	WindowManager
	Any.h
	Enum.h
	Event.h
	DataExchange.h
	Layer.h
	Layer.cpp
	Main.h
	Main.cpp
	Property.h
	Scene.h
	SGuid.h
	SGuid.cpp
	Struct.h
	Threading.h
	TMP.h
	Types.h
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

IF(${CGI})
    LIST(APPEND Local
        CGI
    )
ENDIF()

IF(${SCRIPTING})
	LIST(APPEND Local
		Scripting
	)
ENDIF()


IF(${UI})
	LIST(APPEND Local
		UI
	)
ENDIF()

