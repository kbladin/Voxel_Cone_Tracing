#ifndef MY_ENGINE_H
#define MY_ENGINE_H

#include "SimpleGraphicsEngine.h"

using namespace SGE;

class Planet;
class Quad;
class MyObject3D;

struct Material
{
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	float reflectance; // [0, 1]
	float specular_reflectance; // [0, 1], part of reflectance
	float specular_polish; // [0, 1]
	float radiosity;
};

//! The actual program extending SimpleGraphicsEngine
/*!
	Here, all objects can be added to the scene_ and input is handled outside of
	SimpleGraphicsEngine.
*/
class MyEngine : public SimpleGraphicsEngine {
public:
	MyEngine();
	~MyEngine();
	virtual void update();
	virtual void render();
	static void mouseScrollCallback(GLFWwindow * window, double dx, double dy);
	static void mousePosCallback(GLFWwindow * window, double x, double y);
	static void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow * window, double x, double y);
	static void keyCallback(
		GLFWwindow * window,
		int key,
		int scancode,
		int action,
		int mods);
	void updateCameraController();
	template <class T>
	void Delay(T* input, T end_val, float speed);
private:
	void voxelizeScene();
	void clearVoxels();
	void renderVolume();
	void renderGlobal();
	void renderLocalDiffuse();

	void init3DTexture();

	void createMaterialTweakbar(Material* m);

	GLuint tex3D;

	// Tweakable variables
	static bool mouse_control_;
	float hej;

	// Constants
	int tex_size = 128;
	float scene_scale = 2;


	// FBO
	FBO* fbo1_;
	FBO* fbo2_;

	// Cameras
	static Object3D* camera_;
	OrthoCamera* voxelizer_camera_;
	PerspectiveCamera* basic_cam_;

	// Objects
	//Planet* planet_;
	Quad* quad_;
	TriangleMesh* cube_;
	TriangleMesh* icosphere_;
	TriangleMesh* floor_mesh_;
	TriangleMesh* bunny_mesh_;

	MyObject3D* bunny_;
	MyObject3D* bunny2_;
	MyObject3D* floor_;
	MyObject3D* roof_;
	MyObject3D* l_wall_;
	MyObject3D* r_wall_;
	MyObject3D* b_wall_;
	MyObject3D* light_object_;

	LightSource* light_;

	float roll_goal = 0;
	float pitch_goal = 0;
	float yaw_goal = 0;

	float roll = 0;
	float pitch = 0;
	float yaw = 0;

	glm::vec3 camera_pos; 
	glm::vec3 camera_pos_goal; 
	float xmouse;
	float ymouse;

	float delay_counter_;
	int frame_counter_;

	// Shaders
	GLuint shader_phong_;
	GLuint shader_plaintexture_;
	GLuint shader_simplevolume_;
	GLuint shader_worldpositionoutput_;
	GLuint shader_global_;
	GLuint shader_voxelization_;
	GLuint shader_clearvoxels_;
};

class MyObject3D : public Object3D {
public:
	MyObject3D(Material material);
	~MyObject3D();
	void render(glm::mat4 M, GLuint program_ID);
	Material* getMaterialPointer(){return &material_;};
private:
	Material material_;
};

class Quad : public Object3D {
public:
	Quad();
	~Quad();
private:
	TriangleMesh* mesh_;
};

class Planet : public Object3D {
public:
	Planet();
	~Planet();
private:
	TriangleMesh* mesh_;
	void buildIcosahedron(
		float radius, // Kkm
		std::vector<glm::vec3>* positions,
		std::vector<glm::vec3>* normals,
		std::vector<unsigned short>* elements);
};

#endif