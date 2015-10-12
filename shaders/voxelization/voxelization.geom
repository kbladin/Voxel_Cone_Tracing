#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPositions[];

out vec3 vertexPosition_worldspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
	// Dominant axis projection
	vec3 normal = normalize(cross(
		gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
		gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	float proj_x = abs(dot(normal, vec3(1,0,0)));
	float proj_y = abs(dot(normal, vec3(0,1,0)));
	float proj_z = abs(dot(normal, vec3(0,0,1)));

	if (proj_x > proj_y && proj_x > proj_z)
	{ // x axis is dominant, project on x plane
		vertexPosition_worldspace = vertexPositions[0];
	    gl_Position = vec4(0, gl_in[0].gl_Position.y, gl_in[0].gl_Position.z, gl_in[0].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[1];
	    gl_Position = vec4(0, gl_in[1].gl_Position.y, gl_in[1].gl_Position.z, gl_in[1].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[2];
	    gl_Position = vec4(0, gl_in[2].gl_Position.y, gl_in[2].gl_Position.z, gl_in[2].gl_Position.w);
	    EmitVertex();
	}
	else if (proj_y > proj_x && proj_y > proj_z)
	{ // y axis is dominant, project on y plane
		vertexPosition_worldspace = vertexPositions[0];
	    gl_Position = vec4(gl_in[0].gl_Position.x, 0, gl_in[0].gl_Position.z, gl_in[0].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[1];
	    gl_Position = vec4(gl_in[1].gl_Position.x, 0, gl_in[1].gl_Position.z, gl_in[1].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[2];
	    gl_Position = vec4(gl_in[2].gl_Position.x, 0, gl_in[2].gl_Position.z, gl_in[2].gl_Position.w);
	    EmitVertex();
	}
	else
	{ // z axis is dominant, project on z plane
		vertexPosition_worldspace = vertexPositions[0];
	    gl_Position = vec4(gl_in[0].gl_Position.x, gl_in[0].gl_Position.x, 0, gl_in[0].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[1];
	    gl_Position = vec4(gl_in[1].gl_Position.x, gl_in[0].gl_Position.x, 0, gl_in[1].gl_Position.w);
	    EmitVertex();

		vertexPosition_worldspace = vertexPositions[2];
	    gl_Position = vec4(gl_in[2].gl_Position.x, gl_in[0].gl_Position.x, 0, gl_in[2].gl_Position.w);
	    EmitVertex();
		
	}
    EndPrimitive();
}