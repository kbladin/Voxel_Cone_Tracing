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
