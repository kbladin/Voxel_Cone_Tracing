#ifndef MY_ENGINE_H
#define MY_ENGINE_H

#include "SimpleGraphicsEngine.h"

using namespace SGE;

class Planet;
class Quad;

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
	static void keyCallback(
		GLFWwindow * window,
		int key,
		int scancode,
		int action,
		int mods);
	void updateCameraController();
private:
	// FBO
	// What the hell? If i move these declarations below the others it does not work??!!
	FBO3D* fbo3D_;
	FBO* fbo1_;
	FBO* fbo2_;

	// Cameras
	static Object3D* camera_;
	OrthoCamera* slicer_camera_;
	PerspectiveCamera* basic_cam_;

	// Objects
	Planet* planet_;
	Quad* quad_;
	TriangleMesh* cube_;

	float roll, pitch, yaw;

	float delay_counter_;
	int frame_counter_;

	// Shaders
	GLuint shader_phong_;
	GLuint shader_plaintexture_;
	GLuint shader_simplevolume_;
	GLuint shader_worldpositionoutput_;
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