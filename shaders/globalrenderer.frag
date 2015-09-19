#version 410 core

#define M_PI 3.14159

in vec3 normal_viewspace;
in vec3 vertexPosition_viewspace;
in vec3 vertexPosition_worldspace;
in vec3 normal_worldspace;
in vec3 eyePosition_worldspace;

uniform float time;
uniform int textureSize;
uniform sampler3D texUnit3D;

out vec4 color;

float random(float xx){
	float x0 = floor(xx);
	float x1 = x0+1;
	float v0 = sin( x0*14686 ) * 31718.927 + x0;
	float v1 = sin( x1*14686 ) * 31718.927 + x1;
	v0 = v0 - int(v0);
	v1 = v1 - int(v1);
	float fracxx = xx - int(xx);
	return (v0*(1-fracxx)+v1*(fracxx))*2-1*sin(xx);
}

// Trace cone from position vertexPosition_worldspace and accumulate color
vec3 coneTrace(vec3 rayDirection)
{
	vec4 res = vec4(0,0,0,0);
	float rayStep = float(1) / textureSize * 3;
	vec3 rayOrigin = vertexPosition_worldspace + normalize(normal_worldspace) * rayStep;
	
	for (int mipLevel=0; mipLevel<10; mipLevel++)
	{
		vec3 samplePoint = (rayOrigin + rayDirection * pow(2, mipLevel) * rayStep );
		res += textureLod(texUnit3D, (samplePoint + vec3(1,1,1)) / 2, mipLevel);
		if (res.a > 0.99)
			break;
	}
	return res.rgb;
}

// Trace cone from position vertexPosition_worldspace and accumulate color
vec3 coneTrace2(vec3 rayDirection, float coneAngle, float multiSample)
{
	vec4 res = vec4(0,0,0,0);
	float tanTheta2 = tan(coneAngle / 2);
	float voxelSize = float(1) / textureSize;

	vec3 rayOrigin = vertexPosition_worldspace + voxelSize * 3 * normalize(normal_worldspace);
	float sampleStep = voxelSize;
	float t = voxelSize;
	for (int i=0; i<100; i++)
	{
		float d = (tanTheta2 * t * 2); // Sphere diameter
		float mipLevel = log2(d / voxelSize);
		
		if (mipLevel> 6)
			break;

		vec3 samplePoint = (rayOrigin + rayDirection * t );
		vec4 texSample = textureLod(texUnit3D, (samplePoint + vec3(1,1,1)) / 2, mipLevel);
		
		// Alpha compositing
		res.rgb = texSample.a * texSample.rgb + (1 - texSample.a) * res.rgb;
		res.a = texSample.a + (1 - texSample.a) * res.a;

		if (res.a > 0.99)
			break;

		res += texSample;

		// Increment sampleStep
		sampleStep = sampleStep * (1 + tanTheta2) / (1 - tanTheta2);
		t += sampleStep * multiSample;
	}
	return res.rgb;
}

vec3 calculateLocalDiffuse(vec3 n_viewspace)
{
	vec3 res = vec3(0,0,0);
	vec3 n = n_viewspace;
	vec3 l = normalize(-vec3(1,1,1));

	float cosTheta = dot(n,-l);
	vec3 diffuse = vec3(1,1,1) * max(cosTheta, 0);

	vec3 ambient = vec3(0.0,0.0,0.0);
	res = ambient + diffuse;
	return res;
}

vec3 calculateGlobalDiffuse(vec3 n_worldspace)
{
	vec3 n = n_worldspace;
   	vec3 rayDirection = n;
   	vec3 res;
   	float coneAngle = M_PI / 6;

   	// Pick a random vector helper
   	vec3 helper = normalize(vec3(1,0,0));
   	if (abs(dot(n,helper)) == 1)
   		// Pick a new helper
   		helper = vec3(0,1,0);

   	// Find a tangent and a bitangent
   	vec3 t = normalize(helper - dot(n,helper) * n);
   	vec3 bt = cross(t, n);
   	
   	// First trace a cone in the normal direction
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;

   	// Rotate the ray direction 30 degrees around the tangent to achieve the next
   	// ray direction
   	rayDirection = 0.5 * n + sqrt(3)/2 * t;
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;

   	// Next sample direction
   	rayDirection = 0.5 * n + sqrt(3)/2 * (1/4 * (sqrt(5) - 1) * t + sqrt(5/8 + sqrt(5)/8) * bt);
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;
  	// Next sample direction
   	rayDirection = 0.5 * n + sqrt(3)/2 * (1/4 * (sqrt(5) - 1) * t - sqrt(5/8 + sqrt(5)/8) * bt);
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;
	// Next sample direction
   	rayDirection = 0.5 * n - sqrt(3)/2 * (1/4 * (sqrt(5) + 1) * t + sqrt(5/8 - sqrt(5)/8) * bt);
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;
   	// Next sample direction
   	rayDirection = 0.5 * n - sqrt(3)/2 * (1/4 * (sqrt(5) + 1) * t - sqrt(5/8 - sqrt(5)/8) * bt);
   	res += coneTrace2(rayDirection, coneAngle, 1) / 6;

   	return res;
}

void main(){
	// Local light contribution
    //color.rgb = calculateLocalDiffuse(normalize(normal_viewspace));
	color.a = 1;
	color.rgb = calculateLocalDiffuse(normalize(normal_worldspace));
	color.rgb += calculateGlobalDiffuse(normalize(normal_worldspace));

	vec3 v = normalize( vertexPosition_worldspace - eyePosition_worldspace );
	vec3 r = reflect(v, normalize(normal_worldspace));
	//color.rgb += coneTrace2(r, M_PI / 20, 1);

}