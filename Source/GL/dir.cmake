SET(Local
	../GL.h
	OpenGL.h
	Shader.h
	Shader.cpp
)

IF(${OPENGL})
	LIST(APPEND Local OpenGL.cpp)
ENDIF()