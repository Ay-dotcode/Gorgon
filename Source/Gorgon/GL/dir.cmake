SET(Local
	../GL.h
	OpenGL.h
	Shader.h
	Shader.cpp
)

IF(${OPENGL})

	FIND_PACKAGE(OpenGL REQUIRED)

	LIST(APPEND Libs ${OPENGL_LIBRARIES})
	
	LIST(APPEND Local OpenGL.cpp)
ENDIF()