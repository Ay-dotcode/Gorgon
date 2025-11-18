#version 130

in vec2 texcoord;

uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = vec4(tint.a, 0, 0, 1);
}