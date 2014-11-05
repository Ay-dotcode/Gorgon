SET(Local
	Bounds.h
	Margins.h
	Point.h
	Rectangle.h
	Size.h
)
IF(${SCRIPTING})
	LIST(APPEND Local
		Scripting
	)
ENDIF()