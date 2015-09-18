#version 410 core



in vec2 texCoord;
in vec3 vertexPosition_screenspace_frag;
out vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform int textureSize;

uniform sampler3D texUnit3D;
uniform sampler2D texUnitBackCube;
uniform sampler2D texUnitFrontCube;

void main(){
	vertexPosition_screenspace_frag;
	//vec4 vertexPosition_worldspace = inverse(P * V) * vec4(vertexPosition_screenspace_frag, 1);
	//vertexPosition_worldspace = vertexPosition_worldspace / vertexPosition_worldspace.w;

	//vec3 ray_direction = normalize();
	//vec4 rayOrigin_worldspace = inverse(P * V) * vec4(0,0,0,1);
	//rayOrigin_worldspace = rayOrigin_worldspace / rayOrigin_worldspace.w;

	//vec3 ray_direction = normalize(vertexPosition_worldspace.xyz - rayOrigin_worldspace.xyz);
	



	vec3 rayOrigin = texture(texUnitFrontCube, texCoord).xyz;
	vec3 rayEnd = texture(texUnitBackCube, texCoord).xyz;
	vec3 rayDirection = normalize(rayEnd - rayOrigin);
	float rayStep = 0.01;
	int nSteps = int(length(rayEnd - rayOrigin) / rayStep);

	for (int i=0; i<nSteps; i++)
	{
		int mipLevel = 2;
		vec3 samplePoint = (rayOrigin + rayDirection * rayStep * i);
		samplePoint = vec3(samplePoint.x, samplePoint.y, samplePoint.z);
		color += textureLod(texUnit3D, (samplePoint + vec3(1,1,1)) / 2, mipLevel);	
	}
	//color.rgb = texture(texUnitFrontCube, texCoord).xyz;
    //color.a = 1;
}


/*


in vec2 texCoord;
in vec3 vertexPosition_screenspace_frag;
out vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform sampler3D texUnit3D;

void main(){
	
	vec4 vertexPosition_worldspace = inverse(P * V) * vec4(vertexPosition_screenspace_frag, 1);
	vertexPosition_worldspace = vertexPosition_worldspace / vertexPosition_worldspace.w;

	//vec3 ray_direction = normalize();
	vec4 rayOrigin_worldspace = inverse(P * V) * vec4(0,0,0,1);
	rayOrigin_worldspace = rayOrigin_worldspace / rayOrigin_worldspace.w;

	vec3 ray_direction = normalize(vertexPosition_worldspace.xyz - rayOrigin_worldspace.xyz);
	vec3 ray_origin = rayOrigin_worldspace.xyz;

	for (int i=0; i<100; i++)
	{
		vec3 samplePoint = (ray_origin + ray_direction * float(i)/100);
		samplePoint = vec3(samplePoint.x, samplePoint.y, samplePoint.z);
		color.rgb += texture(texUnit3D, (samplePoint - vec3(1,1,1)) / 2).rgb;
		if (color.r > 0.0)
			break;

	}
    color.a = 1;
}


*/

