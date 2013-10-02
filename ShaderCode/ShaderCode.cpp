// ShaderCode.cpp

#include "ShaderCode.h"

namespace gge { namespace shadercode {	

	SimpleShader::SimpleShader()
	{ 
		InitializeWithSource(TransformVertSrcCode, SimpleFragSrcCode);

		LocateUniform("vertex_coords");
		LocateUniform("tex_coords");
		LocateUniform("diffuse");
		UpdateUniform("diffuse", graphics::TextureUnit::Diffuse);
	}
	
	SimpleTintShader::SimpleTintShader()
	{ 
		InitializeWithSource(TransformVertSrcCode, SimpleTintFragSrcCode);

		LocateUniform("vertex_coords");
		LocateUniform("tex_coords");
		LocateUniform("diffuse");
		LocateUniform("tint");
		UpdateUniform("diffuse", graphics::TextureUnit::Diffuse);
	}

	Shade3DShader::Shade3DShader()
	{ 
		InitializeFromFiles("transform3D.vert", "shade3D.frag");

		LocateUniform("vertex_coords");
		LocateUniform("vertex_coords_view_space");
		LocateUniform("tex_coords");

		LocateUniform("diffuse_tex");
		LocateUniform("normal_tex");
		LocateUniform("depth_tex");	

		LocateUniform("point_light.intensity");
		LocateUniform("point_light.position");
		LocateUniform("ambient");

		LocateUniform("world_to_view");

		UpdateUniform("diffuse_tex",	graphics::TextureUnit::Diffuse);
		UpdateUniform("normal_tex",		graphics::TextureUnit::Normal);
		UpdateUniform("depth_tex",		graphics::TextureUnit::Depth);
	}


	const std::string TransformVertSrcCode = "											\n\
#version 130																			\n\
attribute int vertex_index;												\n\
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
}\n";

} }