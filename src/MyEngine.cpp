#include "../include/MyEngine.h"
#include <iostream>
#include <sstream> 

Object3D* MyEngine::camera_;

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
  ShaderManager::instance()->loadShader(
    "SHADER_SIMPLEVOLUME",
    "../shaders/simplevolumerenderer.vert",
    nullptr, 
    nullptr,
    nullptr,
    "../shaders/simplevolumerenderer.frag");
  ShaderManager::instance()->loadShader(
    "SHADER_WORLDPOSITIONOUTPUT",
    "../shaders/worldpositionoutputshader.vert",
    nullptr, 
    nullptr,
    nullptr,
    "../shaders/worldpositionoutputshader.frag");

  shader_phong_ = ShaderManager::instance()->getShader("SHADER_PHONG");
  shader_plaintexture_ = ShaderManager::instance()->getShader("SHADER_PLAINTEXTURE");
  shader_simplevolume_ = ShaderManager::instance()->getShader("SHADER_SIMPLEVOLUME");
  shader_worldpositionoutput_ = ShaderManager::instance()->getShader("SHADER_WORLDPOSITIONOUTPUT");

  // FBO
  fbo3D_ = new FBO3D(128);
  fbo1_ = new FBO(640, 480, 0);
  fbo2_ = new FBO(640, 480, 0);

  // Cameras
  camera_ = new Object3D();
  basic_cam_ = new PerspectiveCamera(window_);
  slicer_camera_ = new OrthoCamera();

  camera_->addChild(basic_cam_);

  // Objects
  planet_ = new Planet();
  quad_ = new Quad();
  cube_ = new TriangleMesh("../data/meshes/cube.obj");
  scene_->addChild(planet_);
  
  // Set callback functions
  glfwSetScrollCallback(window_, mouseScrollCallback);
  glfwSetKeyCallback(window_, keyCallback);


  // Voxelize the mesh

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1);
  

  glBindFramebuffer(GL_FRAMEBUFFER, fbo3D_->fb_);
  glViewport(0, 0, fbo3D_->size_, fbo3D_->size_);


  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
 
  for (int i = 0; i < fbo3D_->size_; ++i)
  {
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, fbo3D_->texid_, 0, i);    
    float scene_scale = 1;

    slicer_camera_->render(
      glm::mat4(),
      shader_phong_,
      -scene_scale, // left
      scene_scale, // right
      -scene_scale, // bottom
      scene_scale, // top
      scene_scale - (float)i / fbo3D_->size_ * scene_scale * 2, // near
      scene_scale - (float)(i + 1) / fbo3D_->size_ * scene_scale * 2); // far

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene_->render(glm::mat4(), shader_phong_);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, fbo3D_->texid_);
  glGenerateMipmap(GL_TEXTURE_3D);


}

MyEngine::~MyEngine()
{
  delete camera_;
  delete basic_cam_;
  delete slicer_camera_;

  delete planet_;
  delete quad_;
  delete cube_;

  delete fbo3D_;
  delete fbo1_;
  delete fbo2_;
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
  


















  int w, h;
  glfwGetWindowSize(window_, &w, &h);


  // Render back size of cube
  glBindFramebuffer(GL_FRAMEBUFFER, fbo1_->fb_);
  glViewport(0, 0, fbo1_->width_, fbo1_->height_);

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_DEPTH_TEST);

  camera_->render(
      glm::mat4(),
      shader_worldpositionoutput_);
  cube_->render(glm::mat4(), shader_worldpositionoutput_);


  // Render front size of cube
  glBindFramebuffer(GL_FRAMEBUFFER, fbo2_->fb_);
  glViewport(0, 0, fbo2_->width_, fbo2_->height_);

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  camera_->render(
      glm::mat4(),
      shader_worldpositionoutput_);
  cube_->render(glm::mat4(), shader_worldpositionoutput_);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, fbo3D_->texid_);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fbo1_->texid_);


  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, fbo2_->texid_);


  // Render to screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w * 2, h * 2);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);


  glUseProgram(shader_simplevolume_);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "textureSize"), fbo3D_->size_);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnit3D"), 0);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnitBackCube"), 1);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnitFrontCube"), 2);
  camera_->render(
      glm::mat4(),
      shader_simplevolume_);
  quad_->render(glm::mat4(), shader_simplevolume_);

/*
  // Render to screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w * 2, h * 2);

  glUseProgram(shader_phong_);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);


  camera_->render(
      glm::mat4(),
      shader_phong_);
  scene_->render(glm::mat4(), shader_phong_);
*/
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
      roll += 0.02;
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
      roll -= 0.02;
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
      pitch += 0.02;
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
      pitch -= 0.02;

    //slicer_camera_->transform_matrix_ = glm::translate(glm::mat4(), glm::vec3(-roll,0.0f,-pitch));
    camera_->transform_matrix_ = glm::translate(glm::mat4(), glm::vec3(-roll,0.0f,-pitch));
}

Quad::Quad()
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> elements;

  positions.push_back(glm::vec3(-1,-1,1));
  positions.push_back(glm::vec3(1,-1,1));
  positions.push_back(glm::vec3(1,1,1));
  positions.push_back(glm::vec3(-1,1,1));  

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
  
  buildIcosahedron(0.7, &positions, &normals, &elements);

  mesh_ = new TriangleMesh("../data/meshes/bunny.obj");
  //mesh_ = new TriangleMesh(positions, normals, elements);
  this->addChild(mesh_);
  this->transform_matrix_ = glm::scale(glm::mat4(), glm::vec3(0.3,0.3,0.3));
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