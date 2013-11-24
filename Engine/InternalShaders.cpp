// ShaderCode.cpp

#include "InternalShaders.h"
#include <string>

namespace gge { namespace shaders {

	
	const std::string TransformVertSrcCode = "											\n\
#version 130																			\n\
in int vertex_index;																	\n\
																						\n\
uniform mat4x3 vertex_coords;															\n\
uniform mat4x2 tex_coords;																\n\
																						\n\
out vec2 texcoord;																\n\
																						\n\
void main()																				\n\
{																						\n\
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);								\n\
    texcoord = tex_coords[vertex_index];												\n\
}\n";

	const std::string SimpleFragSrcCode = "												\n\
#version 130																			\n\
																						\n\
in vec2 texcoord;																\n\
																						\n\
uniform sampler2D diffuse;																\n\
																						\n\
out vec4 output_color;																	\n\
																						\n\
void main()																				\n\
{																						\n\
    output_color = texture(diffuse, texcoord);											\n\
}\n";
	const std::string SimpleTintFragSrcCode = " 										\n\
#version 130 																			\n\
 																						\n\
in vec2 texcoord; 																\n\
 																						\n\
uniform sampler2D diffuse; 																\n\
uniform vec4 tint; 																		\n\
 																						\n\
out vec4 output_color; 																	\n\
 																						\n\
void main() 																			\n\
{ 																						\n\
    output_color = texture(diffuse, texcoord) * tint; 									\n\
}";

    const std::string MaskedVertexSrcCode = "                                           \n\
#version 130																		    \n\
in int vertex_index;																	\n\
																						\n\
uniform mat4x3 vertex_coords;                                                           \n\
uniform mat4x4 tex_coords;															    \n\
                                                                                        \n\
out vec2 diffuse_texcoord;														        \n\
out vec2 mask_texcoord;                                                                 \n\
                                                                                        \n\
void main()																			    \n\
{							                                                            \n\
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);							    \n\
    vec4 coords = tex_coords[vertex_index];                                             \n\
    diffuse_texcoord = coords.xy;                                                       \n\
    mask_texcoord    = coords.zw;                                                       \n\
}";

    const std::string MaskedFragmentSrcCode = "                                         \n\
#version 130 													                        \n\
 																						\n\
in vec2 diffuse_texcoord;                                                               \n\
in vec2 mask_texcoord;                                                                  \n\
                                                                                        \n\
uniform sampler2D diffuse; 																\n\
uniform sampler2D mask;																	\n\
                                                                                        \n\
out vec4 output_color; 																    \n\
                                                                                        \n\
void main() 																	        \n\
{ 																		                \n\
    output_color = vec4(texture(diffuse, diffuse_texcoord).rgb, texture(diffuse, diffuse_texcoord).a*texture(mask, mask_texcoord).a);                 \n\
}";

    const std::string TintedMaskedFragmentSrcCode = "                                   \n\
#version 130 													                        \n\
 																						\n\
in vec2 diffuse_texcoord;                                                               \n\
in vec2 mask_texcoord;                                                                  \n\
                                                                                        \n\
uniform sampler2D diffuse; 																\n\
uniform sampler2D mask;																	\n\
uniform vec4 tint;                                                                      \n\
                                                                                        \n\
out vec4 output_color; 																    \n\
                                                                                        \n\
void main() 																	        \n\
{ 																		                \n\
    output_color = tint * vec4(texture(diffuse, diffuse_texcoord).rgb, texture(diffuse, diffuse_texcoord).a*texture(mask, mask_texcoord).a);                 \n\
}";


	SimpleShader::SimpleShader()
	{
		InitializeWithSource(TransformVertSrcCode, SimpleFragSrcCode);
	}

	SimpleTintShader::SimpleTintShader()
	{
		InitializeWithSource(TransformVertSrcCode, SimpleTintFragSrcCode);
	}

	MaskedShader::MaskedShader()
	{
	    InitializeWithSource(MaskedVertexSrcCode, MaskedFragmentSrcCode);
	}

	TintedMaskedShader::TintedMaskedShader()
	{
	    InitializeWithSource(MaskedVertexSrcCode, TintedMaskedFragmentSrcCode);
	}


} }
