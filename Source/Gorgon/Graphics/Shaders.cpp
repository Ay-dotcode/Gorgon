// ShaderCode.cpp

#include "Shaders.h"


namespace Gorgon { namespace Graphics {


	const std::string TransformVertSrcCode = R"shader(
#version 130

in int vertex_index;

uniform vec3 vertex_coords[4];
uniform vec2 tex_coords[4];

out vec2 texcoord;

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);

    texcoord = tex_coords[vertex_index];
}
)shader";

	const std::string NoTexVertSrcCode = R"shader(
#version 130

in int vertex_index;

uniform vec3 vertex_coords[4];

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);
}
)shader";

	const std::string SimpleFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;

out vec4 output_color;

void main()
{
	output_color = texture(diffuse, texcoord) * tint;
}
)shader";

	const std::string AlphaFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = vec4(tint.rgb, texture(diffuse, texcoord).a * tint.a);
}
)shader";

	const std::string FillFragSrcCode = R"shader(
#version 130

uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = tint;
}
)shader";

	const std::string TiledAtlasFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;
uniform vec2	  size;

out vec4 output_color;

void main()
{
    output_color = texture(diffuse, vec2(mod(texcoord.x, size.x), mod(texcoord.y, size.y)) * tint;
}
)shader";

	const std::string TiledAtlasAlphaFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = vec4(tint.rgb, texture(diffuse, vec2(mod(texcoord.x, size.x), mod(texcoord.y, size.y)).a * tint.a);
}
)shader";

	const std::string MaskedVertSrcCode = R"shader(
#version 130

in int vertex_index;

uniform vec3 vertex_coords[4];
uniform vec2 tex_coords[4];
uniform vec2 mask_coords[4];

out vec2 diffuse_texcoord;
out vec2 mask_texcoord;

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);

    diffuse_texcoord = tex_coords[vertex_index];
    mask_texcoord = mask_coords[vertex_index];
}
)shader";

    const std::string MaskedFragmentSrcCode = R"shader(
#version 130

in vec2 diffuse_texcoord;
in vec2 mask_texcoord;

uniform sampler2D diffuse;
uniform sampler2D mask;

out vec4 output_color;

void main()
{
    output_color = tint * vec4(texture(diffuse, diffuse_texcoord).rgb, texture(diffuse, diffuse_texcoord).a*texture(mask, mask_texcoord).a);
}
)shader";


	SimpleShader::SimpleShader() : Shader("Gorgon::Graphics::Simple") {
		InitializeWithSource(TransformVertSrcCode, SimpleFragSrcCode);
	}

	AlphaShader::AlphaShader() : Shader("Gorgon::Graphics::Alpha") {
		InitializeWithSource(TransformVertSrcCode, AlphaFragSrcCode);
	}

	FillShader::FillShader() : Shader("Gorgon::Graphics::Fill") {
		InitializeWithSource(NoTexVertSrcCode, FillFragSrcCode);
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
