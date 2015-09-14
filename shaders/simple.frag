#version 410 core

in vec3 normal_viewspace;
in vec3 vertexPosition_viewspace;

out vec4 color;

void main(){
	vertexPosition_viewspace;
	vec3 n = normalize(normal_viewspace);
	vec3 l = normalize(-vec3(1,1,1));

	float cosTheta = dot(n,-l);
	vec3 diffuse = vec3(1,1,1) * max(cosTheta, 0);

	vec3 ambient = vec3(0.2,0.2,0.2);
	color.rgb = ambient + diffuse;
    color.a = 1;
}