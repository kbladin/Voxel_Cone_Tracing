#include "../include/SimpleGraphicsEngine.h"

#include <iostream>
#include <vector>
#include <cmath>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>

#include "../include/ShaderLoader.h"
#include "../include/MeshLoader.h"

using namespace SGE;

ShaderManager* ShaderManager::instance_;

ShaderManager::ShaderManager()
{
}

void ShaderManager::loadShader(
  std::string name,
  const char* vs_src,
  const char* tcs_src,
  const char* tes_src,
  const char* gs_src,
  const char* fs_src)
{
  shader_program_IDs.insert(
    std::pair<std::string,GLuint>(name,
    ShaderLoader::loadShaders(
      vs_src, // Vertex shader file path
      tcs_src, // Tesselation control shader file path
      tes_src, // Tesselation evaluation shader file path
      gs_src, // Geometry shader file path
      fs_src))); // Fragment shader file path
}

ShaderManager::~ShaderManager()
{
  for (std::map<std::string, GLuint>::const_iterator it = shader_program_IDs.begin(); it != shader_program_IDs.end(); it++) {
    glDeleteProgram(it->second);
  }
}

ShaderManager* ShaderManager::instance()
{
  if (!instance_) {
    instance_ = new ShaderManager();
  }
  return instance_;
}

GLuint ShaderManager::getShader(std::string name)
{
  GLuint program_ID = shader_program_IDs[name];
  if (!program_ID)
  {
    std::cout << "ERROR : This name, " << name << " is not a valid shader program name!" << std::endl;
  }
  return program_ID;
}

void Object3D::addChild(Object3D *child)
{
  children.push_back(child);
  child->parent_ = this;
}

void Object3D::removeChild(Object3D *child)
{
  /*
  children.erase(std::remove(children.begin(), children.end(), child), children.end());
  for (int i=0; i<children.size(); i++) {
    children[i]->removeChild(child);
  }
  */
}

void Object3D::render(glm::mat4 M, GLuint program_ID)
{
  for (std::vector<Object3D*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
    (*iter)->render(M * transform_matrix_, program_ID);
  }
}

bool Object3D::intersects(glm::vec3 point)
{
  for (int i = 0; i < this->children.size(); ++i)
  {
    if (AbstractMesh* a = dynamic_cast<AbstractMesh*>(this->children[i]))  {
      return a->intersects(point);
    }
  }
}

bool Object3D::intersects(glm::vec3 origin, glm::vec3 direction, float* t)
{
  float t_min = 10000000;
  bool intersected = false;
  for (int i = 0; i < this->children.size(); ++i)
  {
    float curr_t;
    AbstractMesh* a = dynamic_cast<AbstractMesh*>(this->children[i]);
    if (a && a->intersects(origin, direction, &curr_t) && curr_t < t_min)  {
      intersected = true;
      t_min = curr_t;
    }
  }
  *t = t_min;
  return intersected;
}

Object3D::Object3D()
{
  parent_ = nullptr;
}

glm::mat4 Object3D::getTotalTransform()
{
  // Base case:
  if (!parent_)
  {
    return glm::mat4(1);
  }
  else
  { // Recursion
    return parent_->getTotalTransform() * transform_matrix_;
  }
}

AbstractMesh::AbstractMesh()
{
}

AbstractMesh::~AbstractMesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
}

void AbstractMesh::initialize()
{  
  glGenVertexArrays(1, &vertex_array_ID_);
  glBindVertexArray(vertex_array_ID_);
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
}

bool AbstractMesh::intersects(glm::vec3 point)
{
  return aabb_.intersects(point);
}

bool AbstractMesh::intersects(glm::vec3 origin, glm::vec3 direction, float* t)
{
  return aabb_.intersects(origin, direction, t);
}

TriangleMesh::TriangleMesh() : AbstractMesh()
{
  initialize();
}

TriangleMesh::TriangleMesh(const char* file_path) : AbstractMesh()
{
  std::vector<glm::vec2> tmp_uv;

  loadMesh_assimp(
  file_path,
  elements_,
  vertices_, 
  tmp_uv,
  normals_);

  initialize();
}

TriangleMesh::TriangleMesh(
           std::vector<glm::vec3> vertices,
           std::vector<glm::vec3> normals,
           std::vector<unsigned short> elements) : AbstractMesh()
{
  vertices_ = vertices;
  normals_ = normals;
  elements_ = elements;
  
  initialize();
}

TriangleMesh::~TriangleMesh()
{
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
}

void TriangleMesh::initialize()
{
  AbstractMesh::initialize();

  aabb_ = BoundingBox(this);
  this->addChild(&aabb_);
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(unsigned short), &elements_[0] , GL_STATIC_DRAW);
}

void TriangleMesh::render(glm::mat4 M, GLuint program_ID)
{
  Object3D::render(M, program_ID);
  //material_->render();
  
  glm::mat4 total_transform = M * transform_matrix_;

  // Use our shader
  glUseProgram(program_ID);
  
  glUniformMatrix4fv(glGetUniformLocation(program_ID, "M"), 1, GL_FALSE, &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glVertexAttribPointer(
                        1,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  
  // Draw the triangles !
  glDrawElements(
                 GL_TRIANGLES,      // mode
                 elements_.size(),    // count
                 GL_UNSIGNED_SHORT,   // type
                 (void*)0           // element array buffer offset
                 );
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

BoundingBox::BoundingBox(const AbstractMesh* template_mesh)
{
  float max_x = template_mesh->vertices_[0].x;
  float max_y = template_mesh->vertices_[0].y;
  float max_z = template_mesh->vertices_[0].z;
  
  float min_x = template_mesh->vertices_[0].x;
  float min_y = template_mesh->vertices_[0].y;
  float min_z = template_mesh->vertices_[0].z;
  for (int i = 1; i < template_mesh->vertices_.size(); i++)
  {
    max_x = glm::max(max_x, template_mesh->vertices_[i].x);
    max_y = glm::max(max_y, template_mesh->vertices_[i].y);
    max_z = glm::max(max_z, template_mesh->vertices_[i].z);

    min_x = glm::min(min_x, template_mesh->vertices_[i].x);
    min_y = glm::min(min_y, template_mesh->vertices_[i].y);
    min_z = glm::min(min_z, template_mesh->vertices_[i].z);
  }
  max = glm::vec3(max_x, max_y, max_z);
  min = glm::vec3(min_x, min_y, min_z);
}

BoundingBox::BoundingBox(const Object3D)
{
  
}

BoundingBox::BoundingBox()
{
  min = glm::vec3(0,0,0);
  max = glm::vec3(0,0,0);
}

BoundingBox::~BoundingBox()
{
  
}

bool BoundingBox::intersects(glm::vec3 point)
{
  // Transform to world coordinates
  glm::vec3 min_world = min;// glm::vec3(getTotalTransform() * glm::vec4(min, 1));
  glm::vec3 max_world = max;// glm::vec3(getTotalTransform() * glm::vec4(max, 1));
  return (point.x > min_world.x &&
          point.y > min_world.y &&
          point.z > min_world.z &&
          point.x < max_world.x &&
          point.y < max_world.y &&
          point.z < max_world.z);
}

bool BoundingBox::intersects(glm::vec3 origin, glm::vec3 direction, float* t)
{
  // Transform to world coordinates
  origin = glm::vec3(glm::inverse(getTotalTransform()) * glm::vec4(origin, 1));
  direction = glm::vec3(glm::inverse(getTotalTransform()) * glm::vec4(direction, 0));
  glm::vec3 min_world = min;// glm::vec3(getTotalTransform() * glm::vec4(min, 1));
  glm::vec3 max_world = max;// glm::vec3(getTotalTransform() * glm::vec4(max, 1));

  // r.dir is unit direction vector of ray
  glm::vec3 dirfrac(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
  // lb is the corner of AABB with minimal coordinates - left bottom, 
  // rt is maximal corner
  // r.org is the origin of ray
  float t1 = (min_world.x - origin.x)*dirfrac.x;
  float t2 = (max_world.x - origin.x)*dirfrac.x;
  float t3 = (min_world.y - origin.y)*dirfrac.y;
  float t4 = (max_world.y - origin.y)*dirfrac.y;
  float t5 = (min_world.z - origin.z)*dirfrac.z;
  float t6 = (max_world.z - origin.z)*dirfrac.z;

  float tmin = glm::max(
    glm::max(glm::min(t1, t2), glm::min(t3, t4)),
    glm::min(t5, t6));
  float tmax = glm::min(
    glm::min(glm::max(t1, t2), glm::max(t3, t4)),
    glm::max(t5, t6));

  // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
  if (tmax < 0)
  {
      *t = tmax;
      return false;
  }

  // if tmin > tmax, ray doesn't intersect AABB
  if (tmin > tmax)
  {
      *t = tmax;
      return false;
  }

  *t = tmin;
  return true;
  return false;
}

AbstractCamera::AbstractCamera()
{
  transform_matrix_ = glm::lookAt(
                                glm::vec3(0.0f,0.0f,0.0f),
                                glm::vec3(0.0f,0.0f,-1.0f),
                                glm::vec3(0.0f,1.0f,0.0f));
}

void AbstractCamera::render(glm::mat4 M, GLuint program_ID)
{
  Object3D::render(M * transform_matrix_, program_ID);

  glm::mat4 V = glm::inverse(M * transform_matrix_);
  
  glUseProgram(program_ID);

  glUniformMatrix4fv(glGetUniformLocation(program_ID, "V"), 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(program_ID, "P"), 1, GL_FALSE, &projection_transform_matrix_[0][0]);
}

PerspectiveCamera::PerspectiveCamera(GLFWwindow* window) :
AbstractCamera()
{
  window_ = window;
  
  float aspect = 1;
  if (window_)
  {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    aspect = float(width)/height;
  } 

  projection_transform_matrix_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
}

void PerspectiveCamera::render(glm::mat4 M, GLuint program_ID)
{
  float aspect = 1;
  if (window_)
  {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    aspect = float(width)/height;
  } 

  projection_transform_matrix_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
  
  AbstractCamera::render(M, program_ID);
}

OrthoCamera::OrthoCamera(GLFWwindow* window) :
  AbstractCamera()
{
  window_ = window;
  
  float aspect = 1;
  if (window_)
  {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    aspect = float(width)/height;
  } 
  
  projection_transform_matrix_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -100.0f, 100.0f);
}

void OrthoCamera::render(glm::mat4 M, GLuint program_ID)
{
  
  float aspect = 1;
  if (window_)
  {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    aspect = float(width)/height;
  } 


  projection_transform_matrix_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -100.0f, 100.0f);
  
  AbstractCamera::render(M, program_ID);
}

void OrthoCamera::render(
  glm::mat4 M,
  GLuint program_ID,
  float left,
  float right,
  float bottom,
  float top,
  float near,
  float far)
{
    projection_transform_matrix_ = glm::ortho(left, right, bottom, top, near, far);
    AbstractCamera::render(M, program_ID);
}

LightSource::LightSource()
{
  intensity = 0.8f;
  color = glm::vec3(1.0, 1.0, 1.0);
  position = glm::vec3(0.0, 1.0, 0.0);
}

void LightSource::render(glm::mat4 M, GLuint program_ID)
{
  Object3D::render(M * transform_matrix_, program_ID);
  
  glm::vec4 total_position = M * transform_matrix_ * glm::vec4(position.x, position.y, position.z, 1.0f);
  
  glUseProgram(program_ID);
  
  glUniform3f(glGetUniformLocation(program_ID, "light.position"),total_position.x,total_position.y, total_position.z);
  glUniform1f(glGetUniformLocation(program_ID, "light.intensity"), intensity);
  glUniform3f(glGetUniformLocation(program_ID, "light.color"), color.r, color.g, color.b);
}

//Object3D* SimpleGraphicsEngine::camera_;
//Object3D* SimpleGraphicsEngine::viewspace_ortho_camera_;

GLFWwindow* SimpleGraphicsEngine::window_;
Object3D* SimpleGraphicsEngine::scene_;

SimpleGraphicsEngine::SimpleGraphicsEngine()
{
  initialize();
}

SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  glfwTerminate();
  delete scene_;
  //delete view_space_;
  //delete background_space_;
    
  //delete camera_;
  //delete basic_cam_;
  //delete viewspace_ortho_camera_;
  //delete background_ortho_cam_;
}

bool SimpleGraphicsEngine::initialize()
{
  time_ = glfwGetTime();
  // Initialize the library
  if (!glfwInit())
    return -1;
  // Modern OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create a windowed mode window and its OpenGL context
  window_ = glfwCreateWindow(720 * 2, 480 * 2, "", NULL, NULL);
  if (!window_)
  {
    glfwTerminate();
    return false;
  }
  // Make the window's context current
  glfwMakeContextCurrent(window_);
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));
  
  glewExperimental=true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // Instantiate (needs to be done after creating OpenGL context)
  ShaderManager::instance();

  scene_ = new Object3D();
  //view_space_ = new Object3D();
  //background_space_ = new Object3D();
  
  //camera_ = new Object3D();
  //viewspace_ortho_camera_ = new Object3D();
  //basic_cam_ = new PerspectiveCamera(window_);
  //background_ortho_cam_ = new OrthoCamera(ShaderManager::instance()->getShader("SHADER_BACKGROUND"), window_);
  
  //camera_->addChild(basic_cam_);
  //viewspace_ortho_camera_->addChild(background_ortho_cam_);
  //scene_->addChild(camera_);

  //view_space_->addChild(viewspace_ortho_camera_);
  return true;
}

void SimpleGraphicsEngine::run()
{
  while (!glfwWindowShouldClose(window_))
  {
    
    update();

    render();
    
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void SimpleGraphicsEngine::update()
{
  dt_ = glfwGetTime() - time_;
  time_ = glfwGetTime();
  
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  glViewport(0,0,width * 2,height * 2);
}


void SimpleGraphicsEngine::render()
{

}

void FBO::CHECK_FRAMEBUFFER_STATUS()
{
  GLenum status;
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    fprintf(stderr, "Framebuffer not complete\n");
}

FBO::FBO(int width, int height, int int_method)
{
  width_ = width;
  height_ = height;

  // create objects
  glGenFramebuffers(1, &fb_); // frame buffer id
  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  glGenTextures(1, &texid_);
  fprintf(stderr, "%i \n", texid_);
  glBindTexture(GL_TEXTURE_2D, texid_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (int_method == 0)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid_, 0);

  // Renderbuffer
  // initialize depth renderbuffer
    glGenRenderbuffers(1, &rb_);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_);
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_, height_ );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb_ );
    CHECK_FRAMEBUFFER_STATUS();

  fprintf(stderr, "Framebuffer object %d\n", fb_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO()
{
   //Delete resources
   glDeleteTextures(1, &texid_);
   glDeleteRenderbuffersEXT(1, &rb_);
   //Bind 0, which means render to back buffer, as a result, fb is unbound
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glDeleteFramebuffersEXT(1, &fb_);
}

static int lastw = 0;
static int lasth = 0;

// Obsolete
void updateScreenSizeForFBOHandler(int w, int h)
{
  lastw = w;
  lasth = h;
}

// choose input (textures) and output (FBO)
void FBO::useFBO(FBO *out, FBO *in1, FBO *in2)
{
  GLint curfbo;

// This was supposed to catch changes in viewport size and update lastw/lasth.
// It worked for me in the past, but now it causes problems to I have to
// fall back to manual updating.
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
  if (curfbo == 0)
  {
    GLint viewport[4] = {0,0,0,0};
    GLint w, h;
    glGetIntegerv(GL_VIEWPORT, viewport);
    w = viewport[2] - viewport[0];
    h = viewport[3] - viewport[1];
    if ((w > 0) && (h > 0) && (w < 65536) && (h < 65536)) // I don't believe in 64k pixel wide frame buffers for quite some time
    {
      lastw = viewport[2] - viewport[0];
      lasth = viewport[3] - viewport[1];
    }
  }
  
  if (out != nullptr)
    glViewport(0, 0, out->width_, out->height_);
  else
    glViewport(0, 0, lastw, lasth);

  if (out != nullptr)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, out->fb_);
    glViewport(0, 0, out->width_, out->height_);
  }
  else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE1);
  if (in2 != nullptr)
    glBindTexture(GL_TEXTURE_2D, in2->texid_);
  else
    glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  if (in1 != nullptr)
    glBindTexture(GL_TEXTURE_2D, in1->texid_);
  else
    glBindTexture(GL_TEXTURE_2D, 0);
}














void FBO3D::CHECK_FRAMEBUFFER_STATUS()
{
  GLenum status;
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    fprintf(stderr, "Framebuffer not complete\n");
}

FBO3D::FBO3D(int size)
{
  size_ = size;
  
  // create objects
  glGenFramebuffers(1, &fb_); // frame buffer id
  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  glGenTextures(1, &texid_);
  fprintf(stderr, "%i \n", texid_);
  glBindTexture(GL_TEXTURE_3D, texid_);

  std::vector<float> data;
  data.resize(4 * size*size*size);
  for (int i = 0; i < 4 * size*size*size; ++i)
  {
    data[i] = 0.0f;
  }

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexStorage3D(GL_TEXTURE_3D, 10, GL_RGBA32F, size, size, size);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, size, size, size, 0, GL_RGBA, GL_FLOAT, &data[0]);
  //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, size, size, size, 0, GL_RGBA, GL_FLOAT, NULL);

  glGenerateMipmap(GL_TEXTURE_3D); // Allocate the mipmaps  

  glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texid_, 0, 0);


  // Renderbuffer
  // initialize depth renderbuffer
  glGenRenderbuffers(1, &rb_);
  glBindRenderbuffer(GL_RENDERBUFFER, rb_);
  glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size_, size_ );
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb_ );
  CHECK_FRAMEBUFFER_STATUS();

  fprintf(stderr, "Framebuffer object %d\n", fb_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO3D::~FBO3D()
{
   //Delete resources
   glDeleteTextures(1, &texid_);
   glDeleteRenderbuffersEXT(1, &rb_);
   //Bind 0, which means render to back buffer, as a result, fb is unbound
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glDeleteFramebuffersEXT(1, &fb_);
}

// choose input (textures) and output (FBO)
void FBO3D::useFBO(FBO3D *out, FBO3D *in1, FBO3D *in2)
{
  GLint curfbo;

// This was supposed to catch changes in viewport size and update lastw/lasth.
// It worked for me in the past, but now it causes problems to I have to
// fall back to manual updating.
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
  if (curfbo == 0)
  {
    GLint viewport[4] = {0,0,0,0};
    GLint w, h;
    glGetIntegerv(GL_VIEWPORT, viewport);
    w = viewport[2] - viewport[0];
    h = viewport[3] - viewport[1];
    if ((w > 0) && (h > 0) && (w < 65536) && (h < 65536)) // I don't believe in 64k pixel wide frame buffers for quite some time
    {
      lastw = viewport[2] - viewport[0];
      lasth = viewport[3] - viewport[1];
    }
  }
  
  if (out != nullptr)
    glViewport(0, 0, out->size_, out->size_);
  else
    glViewport(0, 0, lastw, lasth);

  if (out != nullptr)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, out->fb_);
    glViewport(0, 0, out->size_, out->size_);
  }
  else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE1);
  if (in2 != nullptr)
    glBindTexture(GL_TEXTURE_3D, in2->texid_);
  else
    glBindTexture(GL_TEXTURE_3D, 0);
  glActiveTexture(GL_TEXTURE0);
  if (in1 != nullptr)
    glBindTexture(GL_TEXTURE_3D, in1->texid_);
  else
    glBindTexture(GL_TEXTURE_3D, 0);
}