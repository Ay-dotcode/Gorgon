#version 330

uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;

uniform mat3 world_to_view; // for transforming normals

struct PointLight {
	vec3 intensity;
	vec3 position;
};

uniform PointLight point_light;
uniform vec3 ambient;

smooth in vec3 position;
smooth in vec2 texcoord;

out vec4 output_color;

vec3 ShadePointLight(PointLight point_light, vec3 position, vec3 normal, vec3 diffuse)
{
	float	dist		= distance(point_light.position, position);
	vec3	light		= normalize(point_light.position - position);
	float	NdotL		= clamp(dot(light, normal), 0.0f, 1.0f);
	float	attenuation = 1.0f / (1.0f + dist * dist);

	return point_light.intensity * diffuse * NdotL * attenuation;
	//return vec3(position.z * 0.01);
	//return diffuse;
}

void main()
{
    vec4 diffuse	= texture(diffuse_tex, texcoord);
	vec3 normal		= texture(normal_tex, texcoord).xyz;
	float depth		= texture(depth_tex, texcoord).x;
	vec3 pos		= position; pos.z += depth;
	vec3 norm		= world_to_view * normal;

	output_color = vec4(ShadePointLight(point_light, pos, norm, diffuse.xyz) + ambient * diffuse.xyz, diffuse.a);
	//output_color = vec4(0.0f, 1.0f, 0.0f, 0.5f);
}