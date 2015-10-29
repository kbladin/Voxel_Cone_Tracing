#version 450 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 V;

out vec2 texCoord;
out vec3 vertexPosition_screenspace_frag;
out vec3 cameraPosition_worldspace_frag;

void main(){
	// Should do scale and bias on this bad boy
	texCoord = (vertexPosition_modelspace.xy + vec2(1,1)) / 2;
	vertexPosition_screenspace_frag = vec3(
		vertexPosition_modelspace.x,
		vertexPosition_modelspace.y,
		vertexPosition_modelspace.z);

	gl_Position = vec4(vertexPosition_modelspace, 1);
	cameraPosition_worldspace_frag = vec3(inverse(V) * vec4(0,0,0,1));
}