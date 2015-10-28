#ifndef MY_ENGINE_H
#define MY_ENGINE_H

#include "SimpleGraphicsEngine.h"
#include <AntTweakBar.h>

using namespace SGE;

class Planet;
class Quad;
class MyObject3D;
class LightObject3D;

struct Material
{
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	float reflectance; // [0, 1]
	float specular_reflectance; // [0, 1], part of reflectance
	float specular_cone_angle; // [0, 1]
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
	void updateCameraController(float dt);
	template <class T>
	void Delay(T* input, T end_val, float speed);
private:
	void voxelizeScene();
	void clearVoxels();
	void renderVolume();
	void renderGlobal();
	void renderLocalDiffuse();

	void init3DTexture();

	static void createObjectTweakbar(MyObject3D* obj);

	GLuint tex3D;

	static bool mouse_control_;
	static float mouse_x_;
	static float mouse_y_;


	static TwBar* tweakbar_;
	static MyObject3D* selected_obj_;
	// Tweakable variables
	float hej;

	// Constants
	int tex_size = 256;
	float scene_scale = 2;


	// FBO
	FBO* fbo1_;
	FBO* fbo2_;

	// Cameras
	static Object3D* camera_;
	OrthoCamera* voxelizer_camera_;
	static PerspectiveCamera* basic_cam_;

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
	LightObject3D* light_object_;

	//LightSource* light_;

	float roll_goal = 0;
	float pitch_goal = 0;
	float yaw_goal = 0;

	float roll = 0;
	float pitch = 0;
	float yaw = 0;

	glm::vec3 camera_pos; 
	glm::vec3 camera_pos_goal; 
	float xmouse = 0;
	float ymouse = 0;

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
protected:
	Material material_;
};

class Quad : public Object3D {
public:
	Quad();
	~Quad();
private:
	TriangleMesh* mesh_;
};

class LightObject3D : public MyObject3D {
public:
	LightObject3D(TriangleMesh* mesh, Material material);
	~LightObject3D();
	void render(glm::mat4 M, GLuint program_ID);
};

#endif