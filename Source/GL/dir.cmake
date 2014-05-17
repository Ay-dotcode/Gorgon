SET(Local
	../GL.h
	OpenGL.h
)

IF(${OPENGL})
	LIST(APPEND Local OpenGL.cpp)
ENDIF()