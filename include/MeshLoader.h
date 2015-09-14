#ifndef MESH_LOADER
#define MESH_LOADER

#include <vector>
#include <glm/glm.hpp>

bool loadMesh_assimp(
	const char * path,
	std::vector<unsigned short> & out_indices,
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals);

#endif