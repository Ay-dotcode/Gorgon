SET(Local
	Bounds.h
	Margins.h
	Point.h
	Rectangle.h
	Size.h
	Point3D.h
	Transform3D.h
)
IF(${SCRIPTING})
	LIST(APPEND Local
		Scripting.cpp
	)
ENDIF()