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
//layout ( r32ui ) coherent volatile uniform uimage3D voxelImage;
uniform Material material;
uniform LightSource light;
uniform float sceneScale;

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


void imageAtomicFloatAdd( layout ( r32ui ) coherent volatile uimage3D imgUI , ivec3 coords , float val )
{
	uint newVal = floatBitsToUint( val ) ;
	uint prevVal = 0; uint curVal;
	// Loop as long as destination value gets changed by other threads
	while ( ( curVal = imageAtomicCompSwap( imgUI , coords , prevVal , newVal ) ) != prevVal )
	{
		prevVal = curVal;
		newVal = floatBitsToUint(( val + uintBitsToFloat( curVal ) ) );
	}
}

vec4 convRGBA8ToVec4( uint val ) {
	return vec4 ( float (( val & 0x000000FF) ) , float (( val & 0x0000FF00) >> 8U ) , float (( val & 0x00FF0000) >> 16U ) , float (( val & 0xFF000000) >> 24U ) ) ;
}

uint convVec4ToRGBA8( vec4 val ) {
	return ( uint ( val.w ) & 0x000000FF) << 24U | ( uint ( val.z ) & 0x000000FF) << 16U | ( uint ( val.y) & 0x000000FF) << 8U | ( uint ( val.x ) & 0x000000FF) ;
}

void imageAtomicRGBA8Avg( layout ( r32ui ) coherent volatile uimage3D imgUI , ivec3 coords , vec4 val ) {
	val.rgb *= 255.0f;
	// Optimise following calculations
	uint newVal = convVec4ToRGBA8( val );
	uint prevStoredVal = 0; uint curStoredVal;
	// Loop as long as destination value gets changed by other threads
	while ( ( curStoredVal = imageAtomicCompSwap( imgUI , coords , prevStoredVal , newVal ) ) != prevStoredVal)
	{
		prevStoredVal = curStoredVal;
		vec4 rval = convRGBA8ToVec4( curStoredVal );
		rval.xyz =( rval.xyz * rval.w ); // Denormalize
		vec4 curValF = rval + val; // Add new value
		curValF.xyz /= ( curValF.w ); // Renormalize
		newVal = convVec4ToRGBA8( curValF );
	}
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

    imageStore(voxelImage, ivec3(texCoord / sceneScale), vec4(color, 1.0f));
    //imageAtomicRGBA8Avg(voxelImage, texCoord, vec4(color, 1.0f));
}