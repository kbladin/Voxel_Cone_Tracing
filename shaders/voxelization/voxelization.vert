/*
#version 450 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 vertexPosition_worldspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	vertexPosition_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1));
	gl_Position = P * V * M * vec4(vertexPosition_modelspace,1);
}
*/

#version 450 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec2 texCoord;
out vec3 vertexPosition;
out vec3 vertexNormal;

out vec3 normal_viewspace_g;
out vec3 normal_worldspace_g;
out vec3 vertexPosition_viewspace_g;
out vec3 vertexPosition_worldspace_g;
out vec3 eyePosition_worldspace_g;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	//texCoord = (vertexPosition_modelspace.xy + vec2(1,1)) / 2;
	vertexPosition = vec3(M * vec4(vertexPosition_modelspace, 1));
	vertexNormal = vec3(M * vec4(vertexNormal_modelspace, 0));
	gl_Position = P * V * M * vec4(vertexPosition_modelspace,1);

	normal_viewspace_g = vec3(V * M * vec4(vertexNormal_modelspace, 0));
	normal_worldspace_g = vec3(M * vec4(vertexNormal_modelspace, 0));
	vertexPosition_viewspace_g = vec3(V * M * vec4(vertexPosition_modelspace, 1));
	vertexPosition_worldspace_g = vec3(M * vec4(vertexPosition_modelspace, 1));
	eyePosition_worldspace_g = vec3(inverse(V) * vec4(0,0,0,1));

//	gl_Position = vec4(vertexPosition_modelspace, 1);
}