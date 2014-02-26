#version 330

layout(location = 0) in int vertex_index;

uniform mat4x3 vertex_coords;
uniform mat4x3 vertex_coords_view_space;
uniform mat4x2 tex_coords;

smooth out vec3 position;
smooth out vec2 texcoord;

void main()
{
	gl_Position = vec4(vertex_coords[vertex_index], 1.0f);
	position = vertex_coords_view_space[vertex_index];
    texcoord = tex_coords[vertex_index];
}
