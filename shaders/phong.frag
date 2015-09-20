#version 410 core

struct Material
{
	vec3 color_diffuse;
	vec3 color_specular;
	float reflectance; // [0, 1]
	float specular_reflectance; // [0, 1], part of reflectance
	float specular_polish; // [0, 1]
};

in vec3 normal_viewspace;
in vec3 normal_worldspace;
in vec3 vertexPosition_viewspace;

uniform Material material;

out vec4 color;

void main(){
	vertexPosition_viewspace;
	vec3 n = normalize(normal_worldspace);
	vec3 l = normalize(-vec3(1,1,1));

	float cosTheta = dot(n,-l);
	vec3 diffuse = vec3(1,1,1) * max(cosTheta, 0) * material.color_diffuse * material.reflectance * (1 - material.specular_reflectance);

	vec3 ambient = vec3(0,0,0);
	color.rgb = ambient + diffuse;
    color.a = 1;
}