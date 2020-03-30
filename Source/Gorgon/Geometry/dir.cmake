SET(Local
	Bounds.h
	Margin.h
	Line.h
	Point.h
	PointList.h
	PointProperty.h
	Rectangle.h
	Size.h
	SizeProperty.h
	Point3D.h
	Transform3D.h
)
IF(${SCRIPTING})
	LIST(APPEND Local
		Scripting.cpp
	)
ENDIF()
