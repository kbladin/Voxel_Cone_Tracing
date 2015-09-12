#include "../include/MyEngine.h"
#include <iostream>
#include <sstream>

MyEngine::MyEngine() : SimpleGraphicsEngine()
{
  // Shaders
  ShaderManager::instance()->loadShader(
    "SHADER_PHONG",
    "../shaders/simple.vert",
    nullptr,
    nullptr,
    nullptr,
    "../shaders/simple.frag");
  ShaderManager::instance()->loadShader(
    "SHADER_PLAINTEXTURE",
    "../shaders/plaintextureshader.vert",
    nullptr,
    nullptr,
    nullptr,
    "../shaders/plaintextureshader.frag");

  shader_phong_ = ShaderManager::instance()->getShader("SHADER_PHONG");
  shader_plaintexture_ = ShaderManager::instance()->getShader("SHADER_PLAINTEXTURE");
 
  // FBO
  fbo1 = new FBO3D(128);
  fbo2 = new FBO3D(128);
  fbo3 = new FBO3D(128);

  planet_ = new Planet();
  quad_ = new Quad();
  scene_->addChild(planet_);
  
  // Set callback functions
  glfwSetScrollCallback(window_, mouseScrollCallback);
  glfwSetKeyCallback(window_, keyCallback);
}

MyEngine::~MyEngine()
{
  delete planet_;
  delete quad_;
  delete fbo1;
  delete fbo2;
  delete fbo3;
}

void MyEngine::update()
{
  SimpleGraphicsEngine::update();

  updateCameraController();

  frame_counter_++;
  delay_counter_ += dt_;
  
  if (delay_counter_ >= 1.0) {
    std::stringstream title;
    title << frame_counter_ << " FPS";
    glfwSetWindowTitle(window_, title.str().c_str());
    frame_counter_ = 0;
    delay_counter_ = 0;
  }
}

void MyEngine::render()
{
  SimpleGraphicsEngine::render();


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1);
  
  FBO3D::useFBO(fbo1, nullptr, nullptr);

  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
 
  for (int i = 0; i < fbo1->size_; ++i)
  {
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, fbo1->texid_, 0, i);

    //glDisable(GL_DEPTH_TEST);
    //background_space_->render(glm::mat4(), shader_phong_);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene_->render(glm::mat4(), shader_phong_);

    //view_space_->render(glm::mat4(), shader_phong_);
  }



  FBO3D::useFBO(nullptr, fbo1, nullptr);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glUniform1i(glGetUniformLocation(shader_plaintexture_, "texUnit"), 0);
  quad_->render(glm::mat4(), shader_plaintexture_);
}

void MyEngine::mouseScrollCallback(GLFWwindow * window, double dx, double dy)
{

}

void MyEngine::keyCallback(
  GLFWwindow * window,
  int key,
  int scancode,
  int action,
  int mods)
{
}

void MyEngine::updateCameraController()
{
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
      roll += 0.1;
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
      roll -= 0.1;
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
      pitch += 0.1;
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
      pitch -= 0.1;

    camera_->transform_matrix_ = glm::translate(glm::mat4(), glm::vec3(-roll,0.0f,-pitch));
}

Quad::Quad()
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> elements;

  positions.push_back(glm::vec3(-1,-1,0));
  positions.push_back(glm::vec3(1,-1,0));
  positions.push_back(glm::vec3(1,1,0));
  positions.push_back(glm::vec3(-1,1,0));  

  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));  

  elements.push_back(0);
  elements.push_back(1);
  elements.push_back(2);
  elements.push_back(0);  
  elements.push_back(2);
  elements.push_back(3);

  mesh_ = new TriangleMesh(positions, normals, elements);
  this->addChild(mesh_);
}

Quad::~Quad()
{
  delete mesh_;
}

Planet::Planet()
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> elements;
  
  buildIcosahedron(1.5, &positions, &normals, &elements);

  mesh_ = new TriangleMesh(positions, normals, elements);
  this->addChild(mesh_);
}

Planet::~Planet()
{
  delete mesh_;
}

void Planet::buildIcosahedron(
  float radius,
  std::vector<glm::vec3>* positions,
  std::vector<glm::vec3>* normals,
  std::vector<unsigned short>* elements)
{
  // create 12 vertices of a icosahedron
  float t = (1.0 + sqrt(5.0)) / 2.0;

  positions->push_back(glm::normalize(glm::vec3(-1,  t,  0)) * radius);
  positions->push_back(glm::normalize(glm::vec3( 1,  t,  0)) * radius);
  positions->push_back(glm::normalize(glm::vec3(-1, -t,  0)) * radius);
  positions->push_back(glm::normalize(glm::vec3( 1, -t,  0)) * radius);

  positions->push_back(glm::normalize(glm::vec3( 0, -1,  t)) * radius);
  positions->push_back(glm::normalize(glm::vec3( 0,  1,  t)) * radius);
  positions->push_back(glm::normalize(glm::vec3( 0, -1, -t)) * radius);
  positions->push_back(glm::normalize(glm::vec3( 0,  1, -t)) * radius);

  positions->push_back(glm::normalize(glm::vec3( t,  0, -1)) * radius);
  positions->push_back(glm::normalize(glm::vec3( t,  0,  1)) * radius);
  positions->push_back(glm::normalize(glm::vec3(-t,  0, -1)) * radius);
  positions->push_back(glm::normalize(glm::vec3(-t,  0,  1)) * radius);

  for (int i = 0; i < positions->size(); ++i)
  {
    normals->push_back((*positions)[i]);
    (*normals)[i] = glm::normalize((*normals)[i]);
  }

  // create 20 triangles of the icosahedron
  // 5 faces around point 0
  elements->push_back(0);
  elements->push_back(11);
  elements->push_back(5);

  elements->push_back(0);
  elements->push_back(5);
  elements->push_back(1);

  elements->push_back(0);
  elements->push_back(1);
  elements->push_back(7);

  elements->push_back(0);
  elements->push_back(7);
  elements->push_back(10);

  elements->push_back(0);
  elements->push_back(10);
  elements->push_back(11);

  // 5 adjacent faces
  elements->push_back(1);
  elements->push_back(5);
  elements->push_back(9);

  elements->push_back(5);
  elements->push_back(11);
  elements->push_back(4);

  elements->push_back(11);
  elements->push_back(10);
  elements->push_back(2);

  elements->push_back(10);
  elements->push_back(7);
  elements->push_back(6);

  elements->push_back(7);
  elements->push_back(1);
  elements->push_back(8);

  // 5 faces around point 3
  elements->push_back(3);
  elements->push_back(9);
  elements->push_back(4);

  elements->push_back(3);
  elements->push_back(4);
  elements->push_back(2);

  elements->push_back(3);
  elements->push_back(2);
  elements->push_back(6);

  elements->push_back(3);
  elements->push_back(6);
  elements->push_back(8);

  elements->push_back(3);
  elements->push_back(8);
  elements->push_back(9);

  // 5 adjacent faces
  elements->push_back(4);
  elements->push_back(9);
  elements->push_back(5);

  elements->push_back(2);
  elements->push_back(4);
  elements->push_back(11);

  elements->push_back(6);
  elements->push_back(2);
  elements->push_back(10);

  elements->push_back(8);
  elements->push_back(6);
  elements->push_back(7);

  elements->push_back(9);
  elements->push_back(8);
  elements->push_back(1);
}