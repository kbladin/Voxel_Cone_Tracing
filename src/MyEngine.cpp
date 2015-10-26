#include "../include/MyEngine.h"

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <sstream> 
#include <sys/timeb.h>


Object3D* MyEngine::camera_;

MyEngine::MyEngine() : SimpleGraphicsEngine()
{
  // Shaders
  ShaderManager::instance()->loadShader(
    "SHADER_PHONG",
    "../shaders/phong.vert",
    nullptr,
    nullptr,
    nullptr,
    "../shaders/phong.frag");
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
  ShaderManager::instance()->loadShader(
    "SHADER_GLOBALRENDERER",
    "../shaders/globalrenderer.vert",
    nullptr, 
    nullptr,
    nullptr,
    "../shaders/globalrenderer.frag");
  ShaderManager::instance()->loadShader(
    "SHADER_VOXELIZATION",
    "../shaders/voxelization/voxelization.vert", // Vertex shader file path
    nullptr, // Tesselation control shader file path
    nullptr, // Tesselation evaluation shader file path
    "../shaders/voxelization/voxelization.geom", // Geometry shader file path
    "../shaders/voxelization/voxelization.frag"); // Fragment shader file path
  ShaderManager::instance()->loadShader(
    "SHADER_CLEARVOXELS",
    "../shaders/voxelization/voxelization.vert", // Vertex shader file path
    nullptr, // Tesselation control shader file path
    nullptr, // Tesselation evaluation shader file path
    "../shaders/voxelization/voxelization.geom", // Geometry shader file path
    "../shaders/voxelization/clearvoxels.frag"); // Fragment shader file path
  
  shader_phong_ = ShaderManager::instance()->getShader("SHADER_PHONG");
  shader_plaintexture_ = ShaderManager::instance()->getShader("SHADER_PLAINTEXTURE");
  shader_simplevolume_ = ShaderManager::instance()->getShader("SHADER_SIMPLEVOLUME");
  shader_worldpositionoutput_ = ShaderManager::instance()->getShader("SHADER_WORLDPOSITIONOUTPUT");
  shader_global_ = ShaderManager::instance()->getShader("SHADER_GLOBALRENDERER");
  shader_voxelization_ = ShaderManager::instance()->getShader("SHADER_VOXELIZATION");
  shader_clearvoxels_ = ShaderManager::instance()->getShader("SHADER_CLEARVOXELS");

  yaw_goal = pitch_goal = roll_goal = 0;
  yaw = pitch = roll = 0;


  // FBO
  fbo1_ = new FBO(640, 480, 0);
  fbo2_ = new FBO(640, 480, 0);

  // Cameras
  camera_ = new Object3D();
  basic_cam_ = new PerspectiveCamera(window_);
  voxelizer_camera_ = new OrthoCamera();

  camera_->addChild(basic_cam_);

  // Materials
  Material material1;
  material1.color_diffuse = glm::vec3(1,1,1);
  material1.color_specular = glm::vec3(1,1,1);
  material1.reflectance = 1;
  material1.specular_reflectance = 0.0;
  material1.specular_polish = 0.9;
  material1.radiosity = 0.0;

  Material red;
  red.color_diffuse = glm::vec3(1,0.5,0.5);
  red.color_specular = glm::vec3(1,1,1);
  red.reflectance = 1.0;
  red.specular_reflectance = 0.0;
  red.specular_polish = 0.9;
  red.radiosity = 0.0;

  Material green;
  green.color_diffuse = glm::vec3(0.5,1,0.5);
  green.color_specular = glm::vec3(1,1,1);
  green.reflectance = 1.0;
  green.specular_reflectance = 0.0;
  green.specular_polish = 0.9;
  green.radiosity = 0.0;

  Material material2;
  material2.color_diffuse = glm::vec3(0.5,1,1);
  material2.color_specular = glm::vec3(1,1,1);
  material2.reflectance = 1.0;
  material2.specular_reflectance = 0.0;
  material2.specular_polish = 0.99;
  material2.radiosity = 0.0;

  Material material3;
  material3.color_diffuse = glm::vec3(1,1,1);
  material3.color_specular = glm::vec3(1,1,0.5);
  material3.reflectance = 1.0;
  material3.specular_reflectance = 1.0;
  material3.specular_polish = 1;
  material3.radiosity = 0.0;

  Material material_light; 
  material_light.color_diffuse = glm::vec3(1,0.9,0.8);
  material_light.color_specular = glm::vec3(0,0,0);
  material_light.reflectance = 0;
  material_light.specular_reflectance = 0;
  material_light.specular_polish = 0;
  material_light.radiosity = 10;

  // Objects
  //planet_ = new Planet();
  quad_ = new Quad();
  cube_ = new TriangleMesh("../data/meshes/cube.obj");
  icosphere_ = new TriangleMesh("../data/meshes/icosphere.obj");
  floor_mesh_ = new TriangleMesh("../data/meshes/floor.obj");
  bunny_mesh_ = new TriangleMesh("../data/meshes/bunny.obj");

  floor_ = new MyObject3D(material1);
  roof_ = new MyObject3D(material1);
  l_wall_ = new MyObject3D(red);
  r_wall_ = new MyObject3D(green);
  b_wall_ = new MyObject3D(material3);
  bunny_ = new MyObject3D(material3);
  bunny2_ = new MyObject3D(material3);
  light_object_ = new MyObject3D(material_light);

  light_ = new LightSource();

  floor_->addChild(floor_mesh_);
  floor_->transform_matrix_ = glm::translate(glm::vec3(0.0f,-1.0f,0.0f)) * floor_->transform_matrix_;
  
  roof_->addChild(floor_mesh_);
  roof_->transform_matrix_ = glm::translate(glm::vec3(0.0f,1.0f,0.0f)) * roof_->transform_matrix_;
  
  l_wall_->addChild(floor_mesh_);
  l_wall_->transform_matrix_ = glm::rotate(float(M_PI / 2), glm::vec3(0.0f,0.0f,1.0f)) * l_wall_->transform_matrix_;
  l_wall_->transform_matrix_ = glm::translate(glm::vec3(-1.0f,0.0f,0.0f)) * l_wall_->transform_matrix_;
  
  r_wall_->addChild(floor_mesh_);
  r_wall_->transform_matrix_ = glm::rotate(float(M_PI / 2), glm::vec3(0.0f,0.0f,1.0f)) * r_wall_->transform_matrix_;
  r_wall_->transform_matrix_ = glm::translate(glm::vec3(1.0f,0.0f,0.0f)) * r_wall_->transform_matrix_;
  
  b_wall_->addChild(floor_mesh_);
  b_wall_->transform_matrix_ = glm::rotate(float(M_PI / 2), glm::vec3(1.0f,0.0f,0.0f)) * b_wall_->transform_matrix_;
  b_wall_->transform_matrix_ = glm::translate(glm::vec3(0.0f,0.0f,-1.0f)) * b_wall_->transform_matrix_;

  bunny_->addChild(bunny_mesh_);
  bunny_->transform_matrix_ = glm::scale(glm::mat4(), glm::vec3(0.3,0.3,0.3));
  bunny_->transform_matrix_ = glm::translate(glm::vec3(-0.0f,0.0f,0.0f)) * bunny_->transform_matrix_;

  bunny2_->addChild(bunny_mesh_);
  bunny2_->transform_matrix_ = glm::scale(glm::mat4(), glm::vec3(0.3,0.3,0.3));
  bunny2_->transform_matrix_ = glm::translate(glm::vec3(0.5f,0.0f,0.0f)) * bunny2_->transform_matrix_;

  light_object_->addChild(icosphere_);
  light_object_->transform_matrix_ = glm::scale(glm::mat4(), glm::vec3(0.5,0.5,0.5));
  light_object_->transform_matrix_ = glm::translate(glm::vec3(0.0,1.0f,0.0)) * light_object_->transform_matrix_;

  scene_->addChild(floor_);
  scene_->addChild(roof_);
  scene_->addChild(l_wall_);
  scene_->addChild(r_wall_);
  scene_->addChild(b_wall_);

  scene_->addChild(bunny_);
  //scene_->addChild(bunny2_);
  scene_->addChild(light_);
  //scene_->addChild(light_object_);

  // Set callback functions
  glfwSetScrollCallback(window_, mouseScrollCallback);
  glfwSetKeyCallback(window_, keyCallback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  init3DTexture();
  voxelizeScene();
}

MyEngine::~MyEngine()
{
  delete camera_;
  delete basic_cam_;
  delete voxelizer_camera_;
  //delete planet_;
  delete quad_;
  delete cube_;
  delete icosphere_;
  delete floor_mesh_;
  delete bunny_mesh_;

  delete floor_;
  delete roof_;
  delete l_wall_;
  delete r_wall_;
  delete b_wall_;
  delete bunny_;
  delete bunny2_;
  delete light_object_;

  delete light_;

  delete fbo1_;
  delete fbo2_;
}

void MyEngine::update()
{
  clearVoxels();

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

void MyEngine::init3DTexture()
{
  std::vector<float> data;
  data.resize(4 * tex_size*tex_size*tex_size);
  for (int i = 0; i < 4 * tex_size*tex_size*tex_size; ++i)
  {
    data[i] = 0.0f;
  }

  glGenTextures(1, &tex3D);
  glBindTexture(GL_TEXTURE_3D, tex3D);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexStorage3D(GL_TEXTURE_3D, 10, GL_RGBA8, tex_size, tex_size, tex_size);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, tex_size, tex_size, tex_size, 0, GL_RGBA, GL_FLOAT, &data[0]);

  glGenerateMipmap(GL_TEXTURE_3D);
}

void MyEngine::render()
{
  SimpleGraphicsEngine::render();
  
  voxelizeScene();
  //renderVolume();
  renderGlobal();
  //renderLocalDiffuse();
}

void MyEngine::clearVoxels()
{
  glViewport(0, 0, tex_size, tex_size);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glBindTexture(GL_TEXTURE_3D, tex3D);
  glBindImageTexture(0, tex3D, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
  glUseProgram(shader_clearvoxels_);
  glUniform1i(glGetUniformLocation(shader_clearvoxels_, "voxelImage"), 0);
  voxelizer_camera_->render(
      glm::mat4(),
      shader_clearvoxels_, -1, 1, -1, 1, 1, -1);
  scene_->render(glm::mat4(), shader_clearvoxels_);

  glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

  glGenerateMipmap(GL_TEXTURE_3D);
}

void MyEngine::voxelizeScene()
{
  glViewport(0, 0, tex_size, tex_size);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glBindTexture(GL_TEXTURE_3D, tex3D);
  glBindImageTexture(0, tex3D, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
  glUseProgram(shader_voxelization_);
  glUniform1i(glGetUniformLocation(shader_voxelization_, "voxelImage"), 0);
  voxelizer_camera_->render(
      glm::mat4(),
      shader_voxelization_, -1, 1, -1, 1, 1, -1);
  scene_->render(glm::mat4(), shader_voxelization_);

  glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

  glGenerateMipmap(GL_TEXTURE_3D);
}

void MyEngine::renderVolume()
{
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

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  camera_->render(
      glm::mat4(),
      shader_worldpositionoutput_);
  cube_->render(glm::mat4(), shader_worldpositionoutput_);

  int w, h;
  glfwGetWindowSize(window_, &w, &h);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, tex3D);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fbo1_->texid_);


  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, fbo2_->texid_);

  // Render to screen with volume renderer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w, h);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);


  glUseProgram(shader_simplevolume_);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "textureSize"), tex_size);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnit3D"), 0);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnitBackCube"), 1);
  glUniform1i(glGetUniformLocation(shader_simplevolume_, "texUnitFrontCube"), 2);

  camera_->render(
      glm::mat4(),
      shader_simplevolume_);
  quad_->render(glm::mat4(), shader_simplevolume_);
}

void MyEngine::renderGlobal()
{
  int w, h;
  glfwGetWindowSize(window_, &w, &h);

  // Render to screen with global renderer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w, h);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  struct timeb tmb;
  ftime(&tmb);

  glUseProgram(shader_global_);
  glUniform1f(glGetUniformLocation(shader_global_, "time"), tmb.millitm);
  glUniform1i(glGetUniformLocation(shader_global_, "textureSize"), tex_size);
  glUniform1i(glGetUniformLocation(shader_global_, "texUnit3D"), 0);
  camera_->render(
      glm::mat4(),
      shader_global_);
  scene_->render(glm::mat4(), shader_global_);
}

void MyEngine::renderLocalDiffuse()
{
  int w, h;
  glfwGetWindowSize(window_, &w, &h);

  // Render to screen with phong renderer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w, h);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  struct timeb tmb;
  ftime(&tmb);

  glUseProgram(shader_phong_);
  camera_->render(
      glm::mat4(),
      shader_phong_);
  scene_->render(glm::mat4(), shader_phong_);
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
  glm::vec3 camera_pos_diff;
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    camera_pos_diff.x = 0.1;
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    camera_pos_diff.x = -0.1;
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    camera_pos_diff.z = 0.1;
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    camera_pos_diff.z = -0.1;
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window_  , GL_TRUE);

  double xmouse_current, ymouse_current;
  glfwGetCursorPos(window_, &xmouse_current, &ymouse_current);

  float xmouse_diff = xmouse_current - xmouse;
  float ymouse_diff = ymouse_current - ymouse;

  xmouse = xmouse_current;
  ymouse = ymouse_current;

  yaw_goal -= xmouse_diff * 0.002;
  roll_goal -= ymouse_diff * 0.002;

  Delay(&yaw, yaw_goal, 0.4);
  Delay(&roll, roll_goal, 0.4);

  glm::mat4 R =
    glm::rotate(glm::mat4(), yaw, glm::vec3(0,1,0)) *
    glm::rotate(glm::mat4(), roll, glm::vec3(1,0,0));
  camera_pos_goal += glm::vec3( R * glm::vec4(camera_pos_diff * 0.6f, 1) );

  Delay(&camera_pos, camera_pos_goal, 0.4);

  glm::mat4 T = glm::translate(camera_pos);
  
  camera_->transform_matrix_ = glm::inverse(T * R);


  if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    bunny_->transform_matrix_ = bunny_->transform_matrix_ * glm::translate(glm::vec3(0,-0.1,0));
    bunny2_->transform_matrix_ = bunny2_->transform_matrix_ * glm::translate(glm::vec3(0,-0.1,0));
  }
  if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS)
  {
    bunny_->transform_matrix_ = bunny_->transform_matrix_ * glm::translate(glm::vec3(0,0.1,0));
    bunny2_->transform_matrix_ = bunny2_->transform_matrix_ * glm::translate(glm::vec3(0,0.1,0));
  }

}

MyObject3D::MyObject3D(Material material)
{
  material_ = material;
}

MyObject3D::~MyObject3D()
{

}

void MyObject3D::render(glm::mat4 M, GLuint program_ID)
{
  glUniform3f(glGetUniformLocation(program_ID, "material.color_diffuse"), material_.color_diffuse.x, material_.color_diffuse.y, material_.color_diffuse.z);
  glUniform3f(glGetUniformLocation(program_ID, "material.color_specular"), material_.color_specular.x, material_.color_specular.y, material_.color_specular.z);
  glUniform1f(glGetUniformLocation(program_ID, "material.reflectance"), material_.reflectance);
  glUniform1f(glGetUniformLocation(program_ID, "material.specular_reflectance"), material_.specular_reflectance);
  glUniform1f(glGetUniformLocation(program_ID, "material.specular_polish"), material_.specular_polish);
  glUniform1f(glGetUniformLocation(program_ID, "material.radiosity"), material_.radiosity);

  Object3D::render(M, program_ID);
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

  //mesh_ = new TriangleMesh("../data/meshes/bunny.obj");
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

template <class T>
void MyEngine::Delay(T* input, T end_val, float speed) {
  if (speed < 0.0f && speed > 1.0f) {
    speed = glm::clamp(speed, 0.0f, 1.0f);
    std::cout << "WARNING: clamping speed between 0.0 and 1.0" << std::endl;
  }
  *input = (end_val - *input) * speed + *input;
}