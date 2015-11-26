#version 450 core

#define gammacorr(x) pow(x, 1/2.6)

in vec2 texCoord;
in vec3 vertexPosition_screenspace_frag;
in vec3 cameraPosition_worldspace_frag;

out vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform int textureSize;
uniform float sceneScale;

uniform sampler3D texUnit3D;
uniform sampler2D texUnitBackCube;
uniform sampler2D texUnitFrontCube;

void main(){
	vec4 res = vec4(0,0,0,0);
	bool insideCube =
		cameraPosition_worldspace_frag.x < (sceneScale + 0.1) &&
		cameraPosition_worldspace_frag.x > -(sceneScale + 0.1) &&
		cameraPosition_worldspace_frag.y < (sceneScale + 0.1) &&
		cameraPosition_worldspace_frag.y > -(sceneScale + 0.1) &&
		cameraPosition_worldspace_frag.z < (sceneScale + 0.1) &&
		cameraPosition_worldspace_frag.z > -(sceneScale + 0.1);
	vec3 rayOrigin;
	if (insideCube)
		rayOrigin = cameraPosition_worldspace_frag;
	else
		rayOrigin = texture(texUnitFrontCube, texCoord).xyz;
	vec3 rayEnd = texture(texUnitBackCube, texCoord).xyz;
	vec3 rayDirection = normalize(rayEnd - rayOrigin);
	float rayStep = 0.002;
	int nSteps = int(length(rayEnd - rayOrigin) / rayStep);

	for (int i=0; i<nSteps; i++)
	{
		float mipLevel = 0;
		vec3 samplePoint = (rayOrigin + rayDirection * rayStep * i);
		//samplePoint = vec3(samplePoint.x, samplePoint.y, samplePoint.z);
		vec4 texSample = textureLod(texUnit3D, (samplePoint / sceneScale + vec3(1,1,1)) / 2, mipLevel);	


		if (texSample.a > 0)
		{
			texSample.rgb /= texSample.a;
			// Alpha compositing
			res.rgb = res.rgb + (1 - res.a) * texSample.a * texSample.rgb;
	        res.a   = res.a   + (1 - res.a) * texSample.a;
		}
		if (res.a > 0.95)
			break;
	}
	color = res;
	color.rgb = vec3(gammacorr(color.r), gammacorr(color.g), gammacorr(color.b));
}
