#version 130

in int vertex_index;

uniform vec3 vertex_coords[4];

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);
}
