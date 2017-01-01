SET(Local
	../GL.h
	FrameBuffer.h
	FrameBuffer.cpp
	OpenGL.h
	Shader.h
	Shader.cpp
	Simple.h
)

IF(${OPENGL})

	FIND_PACKAGE(OpenGL REQUIRED)

	LIST(APPEND Libs ${OPENGL_LIBRARIES})
	
	LIST(APPEND Local OpenGL.cpp)
ENDIF()