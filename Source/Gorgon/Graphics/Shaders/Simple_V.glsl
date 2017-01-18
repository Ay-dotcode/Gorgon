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
 