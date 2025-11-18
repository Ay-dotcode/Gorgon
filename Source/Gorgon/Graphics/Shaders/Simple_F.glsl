#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;

out vec4 output_color;

void main()
{
	output_color = texture(diffuse, texcoord) * tint;
}
