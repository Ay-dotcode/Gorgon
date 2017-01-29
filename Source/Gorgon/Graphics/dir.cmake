SET(Local
	../Graphics.h
	Animations.h
	Bitmap.h
	Bitmap.cpp
	ImageAnimation.h
	Color.h
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
	Pointer.h
	Pointer.cpp
	Shaders.h
	Shaders.cpp
	Texture.h
	TextureTargets.h
)

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