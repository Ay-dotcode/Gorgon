// ShaderCode.cpp

#include "Shaders.h"


namespace Gorgon { namespace Graphics {

#include "ShaderSrc.strings.gen"
	

	SimpleShader::SimpleShader() : Shader("Gorgon::Graphics::Simple") {
		InitializeWithSource(Simple_V, Simple_F);
	}

	AlphaShader::AlphaShader() : Shader("Gorgon::Graphics::Alpha") {
		InitializeWithSource(Simple_V, Alpha_F);
	}

	FillShader::FillShader() : Shader("Gorgon::Graphics::Fill") {
		InitializeWithSource(NoTex_V, Fill_F);
	}

	/*
	MaskedShader::MaskedShader() : Shader("Gorgon::Graphics::Masked")
	{
	    InitializeWithSource(MaskedVertexSrcCode, MaskedFragmentSrcCode);
	}

	TintedMaskedShader::TintedMaskedShader() : Shader("Gorgon::Graphics::MaskedTint")
	{
	    InitializeWithSource(MaskedVertexSrcCode, TintedMaskedFragmentSrcCode);
	}
	*/

} }
