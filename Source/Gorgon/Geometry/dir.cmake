SET(Local
	Bounds.h
	Margin.h
	Point.h
	Line.h
	PointList.h
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
