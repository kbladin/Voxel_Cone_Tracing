#version 450
#extension GL_ARB_shader_image_load_store : enable

struct Material
{
	vec3 color_diffuse;
	vec3 color_specular;
	float reflectance; // [0, 1]
	float specular_reflectance; // [0, 1], part of reflectance
	float specular_polish; // [0, 1]
	float radiosity;
};

struct LightSource
{
	float intensity;
	vec3 color;
	vec3 position;
};

in vec3 normal_viewspace_f;
in vec3 normal_worldspace_f;
in vec3 vertexPosition_viewspace_f;
in vec3 vertexPosition_worldspace_f;
in vec3 eyePosition_worldspace_f;

layout(RGBA8) uniform image3D voxelImage;
uniform Material material;
uniform LightSource light;

vec3 calculateLocalDiffuse()
{
	vec3 n = normalize(normal_worldspace_f);
	vec3 light_diff = vertexPosition_worldspace_f - light.position;
	float light_dist = length(light_diff);
	vec3 l = normalize(light_diff);


	float cosTheta = dot(n,-l);
	vec3 diffuse =
		light.color * light.intensity *
		max(cosTheta, 0) *
		1 / pow(light_dist, 2);

	return diffuse;
}

void main() {
    ivec3 size = imageSize(voxelImage);
	ivec3 texCoord = ivec3((vertexPosition_worldspace_f + vec3(1)) / 2 * size);

	vec3 color;
	if (material.reflectance != 0 && (1 - material.specular_reflectance) != 0)
	{
		color = calculateLocalDiffuse() * material.color_diffuse * material.reflectance * (1 - material.specular_reflectance);
	}
	color += material.radiosity * material.color_diffuse;

    imageStore(voxelImage, texCoord, vec4(color, 1.0f));
}