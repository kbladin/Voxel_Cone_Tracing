#version 410 core

in vec3 vertexPosition_worldspace;

out vec4 color;

void main(){

	color.rgb = vertexPosition_worldspace;
    color.a = 1;
}