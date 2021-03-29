SET(Local
	../Graphics.h
	Animations.h
	Bitmap.h
	Bitmap.cpp
	BlankImage.h
	Color.h
	Color.cpp
	ColorSpaces.h
	ColorSpaces.cpp
	Drawables.h
	EmptyImage.h
	Font.h
	Font.cpp
	BitmapFont.h
	BitmapFont.cpp
	Graphics.cpp
	Layer.cpp
	Layer.h
	Line.h
	Line.cpp
	MaskedObject.h
	Pointer.h
	Pointer.cpp
	Rectangle.h
	Rectangle.cpp
	ScalableObject.h
	StackedObject.h
	Shaders.h
	Shaders.cpp
	Texture.h
	TextureAnimation.h
	TextureTargets.h
	TintedObject.h
	HTMLRenderer.h
	HTMLRenderer.cpp
)


if(FREETYPE STREQUAL BUILTIN)
    list(APPEND Local FreeType.h FreeType.cpp)
    
    set(FREETYPE_INCLUDE_DIRS  "${CMAKE_SOURCE_DIR}/Source/External/freetype/include")
endif()

if(FREETYPE STREQUAL SYSTEM)
    list(APPEND Local FreeType.h FreeType.cpp)

	find_package(Freetype REQUIRED)

	list(APPEND Libs ${FREETYPE_LIBRARIES})   
	
	include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

EmbedShaders(ShaderSrc.strings.gen Shaders.cpp 
	"Shaders/Simple_V.glsl"
	"Shaders/NoTex_V.glsl"	
	"Shaders/Simple_F.glsl"	
	"Shaders/Alpha_F.glsl"	
	"Shaders/Fill_F.glsl"	
	"Shaders/ToMask_F.glsl"	
	"Shaders/ToMaskFill_F.glsl"	
)

EmbedShaders(MaskedShaderSrc.strings.gen Shaders.cpp 
	"Shaders/Masked_V.glsl"
	"Shaders/MaskedAlpha_F.glsl"	
	"Shaders/MaskedNoTex_V.glsl"	
	"Shaders/MaskedFill_F.glsl"
	"Shaders/Masked_F.glsl"
)
