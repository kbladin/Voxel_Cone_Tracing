#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPositions[];

out vec3 vertexPosition_worldspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
	vertexPosition_worldspace = vertexPositions[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

	vertexPosition_worldspace = vertexPositions[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

	vertexPosition_worldspace = vertexPositions[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}