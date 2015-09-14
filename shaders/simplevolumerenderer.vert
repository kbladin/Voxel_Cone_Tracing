#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec2 texCoord;
out vec3 vertexPosition_screenspace_frag;

void main(){
	// Should do scale and bias on this bad boy
	texCoord = (vertexPosition_modelspace.xy + vec2(1,1)) / 2;
	vertexPosition_screenspace_frag = vec3(
		vertexPosition_modelspace.x,
		vertexPosition_modelspace.y,
		vertexPosition_modelspace.z);

	gl_Position = vec4(vertexPosition_modelspace, 1);
}