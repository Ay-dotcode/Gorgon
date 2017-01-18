#version 130

in vec2 texcoord;

uniform sampler2D diffuse;
uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = vec4(tint.rgb, texture(diffuse, texcoord).a * tint.a);
}