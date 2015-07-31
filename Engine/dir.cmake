SET(Local Animation.cpp Animation.h CustomLayer.cpp CustomLayer.h Font.cpp Font.h FontRenderer.h GGE.h GGEMain.cpp GGEMain.h Graphic2D.cpp Graphic2D.h GraphicLayers.cpp GraphicLayers.h GraphicTargets2D.h Graphics.cpp Graphics.h Image.cpp Image.h Input.cpp Input.h InternalShaders.cpp InternalShaders.h Layer.cpp Layer.h OS.Linux.cpp OS.Linux.h OS.Win32.cpp OS.Win32.h OS.cpp OS.h OpenGL.cpp OpenGL.h Pointer.cpp Pointer.h ShaderBase.cpp ShaderBase.h Sound.cpp Sound.h )

FIND_PACKAGE(OpenGL REQUIRED)

LIST(APPEND Libs ${OPENGL_LIBRARIES})