#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 normal_viewspace_g[];
in vec3 normal_worldspace_g[];
in vec3 vertexPosition_viewspace_g[];
in vec3 vertexPosition_worldspace_g[];
in vec3 eyePosition_worldspace_g[];

out vec3 normal_viewspace_f;
out vec3 normal_worldspace_f;
out vec3 vertexPosition_viewspace_f;
out vec3 vertexPosition_worldspace_f;
out vec3 eyePosition_worldspace_f;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
	// Dominant axis projection
	vec3 normal = normalize(cross(
		vertexPosition_worldspace_g[1] - vertexPosition_worldspace_g[0],
		vertexPosition_worldspace_g[2] - vertexPosition_worldspace_g[0]));
	float proj_x = abs(normal.x);
	float proj_y = abs(normal.y);
	float proj_z = abs(normal.z);

	if (proj_x > proj_y && proj_x > proj_z)
	{ // x axis is dominant, project on x plane
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		normal_viewspace_f = normal_viewspace_g[0];
		normal_worldspace_f = normal_worldspace_g[0];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[0];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		eyePosition_worldspace_f = eyePosition_worldspace_g[0];

	    gl_Position = vec4(vertexPosition_worldspace_g[0].y, vertexPosition_worldspace_g[0].z, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		normal_viewspace_f = normal_viewspace_g[1];
		normal_worldspace_f = normal_worldspace_g[1];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[1];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		eyePosition_worldspace_f = eyePosition_worldspace_g[1];

	    gl_Position = vec4(vertexPosition_worldspace_g[1].y, vertexPosition_worldspace_g[1].z, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		normal_viewspace_f = normal_viewspace_g[2];
		normal_worldspace_f = normal_worldspace_g[2];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[2];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		eyePosition_worldspace_f = eyePosition_worldspace_g[2];

	    gl_Position = vec4(vertexPosition_worldspace_g[2].y, vertexPosition_worldspace_g[2].z, 0, 1);
	    EmitVertex();
	}
	else if (proj_y > proj_x && proj_y > proj_z)
	{ // y axis is dominant, project on y plane
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		normal_viewspace_f = normal_viewspace_g[0];
		normal_worldspace_f = normal_worldspace_g[0];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[0];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		eyePosition_worldspace_f = eyePosition_worldspace_g[0];

	    gl_Position = vec4(vertexPosition_worldspace_g[0].x, vertexPosition_worldspace_g[0].z, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		normal_viewspace_f = normal_viewspace_g[1];
		normal_worldspace_f = normal_worldspace_g[1];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[1];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		eyePosition_worldspace_f = eyePosition_worldspace_g[1];

	    gl_Position = vec4(vertexPosition_worldspace_g[1].x, vertexPosition_worldspace_g[1].z, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		normal_viewspace_f = normal_viewspace_g[2];
		normal_worldspace_f = normal_worldspace_g[2];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[2];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		eyePosition_worldspace_f = eyePosition_worldspace_g[2];

	    gl_Position = vec4(vertexPosition_worldspace_g[2].x, vertexPosition_worldspace_g[2].z, 0, 1);
	    EmitVertex();
	}
	else
	{ // z axis is dominant, project on z plane
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		normal_viewspace_f = normal_viewspace_g[0];
		normal_worldspace_f = normal_worldspace_g[0];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[0];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[0];
		eyePosition_worldspace_f = eyePosition_worldspace_g[0];

	    gl_Position = vec4(vertexPosition_worldspace_g[0].x, vertexPosition_worldspace_g[0].y, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		normal_viewspace_f = normal_viewspace_g[1];
		normal_worldspace_f = normal_worldspace_g[1];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[1];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[1];
		eyePosition_worldspace_f = eyePosition_worldspace_g[1];

	    gl_Position = vec4(vertexPosition_worldspace_g[1].x, vertexPosition_worldspace_g[1].y, 0, 1);
	    EmitVertex();

		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		normal_viewspace_f = normal_viewspace_g[2];
		normal_worldspace_f = normal_worldspace_g[2];
		vertexPosition_viewspace_f = vertexPosition_viewspace_g[2];
		vertexPosition_worldspace_f = vertexPosition_worldspace_g[2];
		eyePosition_worldspace_f = eyePosition_worldspace_g[2];

	    gl_Position = vec4(vertexPosition_worldspace_g[2].x, vertexPosition_worldspace_g[2].y, 0, 1);
	    EmitVertex();
		
	}
    EndPrimitive();
}