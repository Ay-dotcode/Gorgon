#version 130

in vec2 texcoord;
in vec2 maskcoord;

uniform sampler2D mask;
uniform vec4      tint;

out vec4 output_color;

void main()
{
    output_color = vec4(tint.rgb, tint.a * texture(mask, maskcoord).a * texture(mask, maskcoord).r);
}