#version 450 core
//#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec2 texCoord;

void main(){
	texCoord = (vertexPosition_modelspace.xy + vec2(1,1)) / 2;

	gl_Position = vec4(vertexPosition_modelspace, 1);
}



