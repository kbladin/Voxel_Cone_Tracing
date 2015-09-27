#version 410 core

struct Material
{
	vec3 color_diffuse;
	vec3 color_specular;
	float reflectance; // [0, 1]
	float specular_reflectance; // [0, 1], part of reflectance
	float specular_polish; // [0, 1]
};

struct LightSource
{
	float intensity;
	vec3 color;
	vec3 position;
};

in vec3 normal_viewspace;
in vec3 normal_worldspace;
in vec3 vertexPosition_viewspace;
in vec3 vertexPosition_worldspace;

uniform Material material;
uniform LightSource light;

out vec4 color;


vec3 calculateLocalDiffuse()
{
	vec3 res = vec3(0,0,0);
	vec3 n = normalize(normal_worldspace);
	vec3 light_diff = vertexPosition_worldspace - light.position;
	float light_dist = length(light_diff);
	vec3 l = normalize(light_diff);


	float cosTheta = dot(n,-l);
	vec3 diffuse =
		light.color * light.intensity *
		max(cosTheta, 0) *
		1 / pow(light_dist, 2);

	vec3 ambient = vec3(0.0,0.0,0.0);
	res = ambient + diffuse;
	return res;
}

void main(){
	color.rgb = calculateLocalDiffuse() * material.color_diffuse * material.reflectance * (1 - material.specular_reflectance);
    color.a = 1;
}