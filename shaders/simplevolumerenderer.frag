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
	vec4 res = vec4(0,0,0,0);
	vec3 rayOrigin = texture(texUnitFrontCube, texCoord).xyz;
	vec3 rayEnd = texture(texUnitBackCube, texCoord).xyz;
	vec3 rayDirection = normalize(rayEnd - rayOrigin);
	float rayStep = 0.01;
	int nSteps = int(length(rayEnd - rayOrigin) / rayStep);

	for (int i=0; i<nSteps; i++)
	{
		float mipLevel = 0;
		vec3 samplePoint = (rayOrigin + rayDirection * rayStep * i);
		samplePoint = vec3(samplePoint.x, samplePoint.y, samplePoint.z);
		vec4 texSample = textureLod(texUnit3D, (samplePoint + vec3(1,1,1)) / 2, mipLevel);	


		if (texSample.a > 0)
		{
			texSample.rgb /= texSample.a;
			// Alpha compositing
			res.rgb = res.rgb + (1 - res.a) * texSample.a * texSample.rgb;
	        res.a   = res.a   + (1 - res.a) * texSample.a;
		}
		if (res.a > 0.8)
			break;
	}
	color = res;
}
