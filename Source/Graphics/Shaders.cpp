// ShaderCode.cpp

#include "Shaders.h"


namespace Gorgon { namespace Graphics {


	const std::string TransformVertSrcCode = R"shader(
#version 130

in int vertex_index;

uniform mat4x3 vertex_coords;
uniform mat4x2 tex_coords;

out vec2 texcoord;

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);
    texcoord = tex_coords[vertex_index];
}
)shader";

	const std::string SimpleFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;

out vec4 output_color;

void main()
{
    output_color = texture(diffuse, texcoord);
}
)shader";

	const std::string SimpleTintFragSrcCode = R"shader(
#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4 tint;

out vec4 output_color;

void main()
{
    output_color = texture(diffuse, texcoord) * tint;
}
)shader";

	const std::string MaskedVertexSrcCode = R"shader(
#version 130
in int vertex_index;

uniform mat4x3 vertex_coords;
uniform mat4x4 tex_coords;

out vec2 diffuse_texcoord;
out vec2 mask_texcoord;

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);
    vec4 coords = tex_coords[vertex_index];
    diffuse_texcoord = coords.xy;
    mask_texcoord    = coords.zw;
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
    output_color = vec4(texture(diffuse, diffuse_texcoord).rgb, texture(diffuse, diffuse_texcoord).a*texture(mask, mask_texcoord).a);
}
)shader";

    const std::string TintedMaskedFragmentSrcCode = R"shader(
#version 130

in vec2 diffuse_texcoord;
in vec2 mask_texcoord;

uniform sampler2D diffuse;
uniform sampler2D mask;
uniform vec4 tint;

out vec4 output_color;

void main()
{
    output_color = tint * vec4(texture(diffuse, diffuse_texcoord).rgb, texture(diffuse, diffuse_texcoord).a*texture(mask, mask_texcoord).a);
}
)shader";

	SimpleShader::SimpleShader() : Shader("Gorgon::Graphics::Simple")
	{
		InitializeWithSource(TransformVertSrcCode, SimpleFragSrcCode);
	}

	SimpleTintShader::SimpleTintShader() : Shader("Gorgon::Graphics::Tint")
	{
		InitializeWithSource(TransformVertSrcCode, SimpleTintFragSrcCode);
	}

	MaskedShader::MaskedShader() : Shader("Gorgon::Graphics::Masked")
	{
	    InitializeWithSource(MaskedVertexSrcCode, MaskedFragmentSrcCode);
	}

	TintedMaskedShader::TintedMaskedShader() : Shader("Gorgon::Graphics::MaskedTint")
	{
	    InitializeWithSource(MaskedVertexSrcCode, TintedMaskedFragmentSrcCode);
	}


} }
