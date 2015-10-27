#ifndef SIMPLE_GRAPHICS_ENGINE_H
#define SIMPLE_GRAPHICS_ENGINE_H

#include <vector>
#include <string>
#include <map>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SGE {
  //! ShaderManager
  class ShaderManager {
  public:
    static ShaderManager* instance();
    ~ShaderManager();
    void loadShader(
      std::string name,
      const char* vs_src,
      const char* tcs_src,
      const char* tes_src,
      const char* gs_src,
      const char* fs_src);
    GLuint getShader(std::string name);
  private:
    ShaderManager();
    static ShaderManager* instance_;
    std::map<std::string, GLuint> shader_program_IDs;
  };
  
  //! Object3D
  class Object3D {
  public:
    Object3D() {};
    virtual ~Object3D() {};
    void addChild(Object3D* child);
    void removeChild(Object3D* child);
    virtual void render(glm::mat4 M, GLuint program_ID);

    bool intersects(glm::vec3 point);
    bool intersects(glm::vec3 origin, glm::vec3 direction, float* t);

    glm::mat4 transform_matrix_;

    std::vector<Object3D*> children;
  protected:
  private:
  };

  class AbstractMesh;

  //! An axis aligned bounding box.
  class BoundingBox : public Object3D{
  public:
    BoundingBox(const AbstractMesh* template_mesh);
    BoundingBox(const Object3D);
    BoundingBox();
    ~BoundingBox();
    glm::vec3 getMin(){return min;}
    glm::vec3 getMax(){return max;}
    bool intersects(glm::vec3 point);
    bool intersects(glm::vec3 origin, glm::vec3 direction, float* t);
  private:
    glm::vec3 max;
    glm::vec3 min;
  };

  //! AbstractMesh
  class AbstractMesh : public Object3D{
  public:
    AbstractMesh();
    ~AbstractMesh();
    virtual void render(glm::mat4 M, GLuint program_ID) = 0;
    bool intersects(glm::vec3 point);
    bool intersects(glm::vec3 origin, glm::vec3 direction, float* t);

    std::vector<glm::vec3> vertices_;
  protected:
    virtual void initialize() = 0;

    BoundingBox aabb_;
    
    GLuint vertex_array_ID_;
    GLuint vertex_buffer_;
  private:
  };
  //! TriangleMesh
  class TriangleMesh : public AbstractMesh{
  public:
    TriangleMesh();
    TriangleMesh(const char* file_path);
    TriangleMesh(std::vector<glm::vec3> vertices,
                 std::vector<glm::vec3> normals,
                 std::vector<unsigned short> elements);
    ~TriangleMesh();
    virtual void render(glm::mat4 M, GLuint program_ID);
  protected:
    void initialize();
    std::vector<unsigned short> elements_; // Maximum around 60000 vertices for unsigned short.
    GLuint element_buffer_;
    GLuint normal_buffer_;
    std::vector<glm::vec3> normals_;
  };
  
  //! AbstractCamera
  class AbstractCamera : public Object3D {
  public:
    AbstractCamera();
    virtual void render(glm::mat4 M, GLuint program_ID) = 0;
    glm::mat4 projection_transform_matrix_;
  protected:  
  };
  //! PerspectiveCamera
  class PerspectiveCamera : public AbstractCamera {
  public:
    PerspectiveCamera(GLFWwindow* window = nullptr);
    virtual void render(glm::mat4 M, GLuint program_ID);
  private:
    GLFWwindow* window_;
  };
  //! OrthoCamera
  class OrthoCamera : public AbstractCamera {
  public:
    OrthoCamera(GLFWwindow* window = nullptr);
    virtual void render(
      glm::mat4 M,
      GLuint program_ID);
    virtual void render(
      glm::mat4 M,
      GLuint program_ID,
      float left,
      float right,
      float bottom,
      float top,
      float near,
      float far);
  private:
    GLFWwindow* window_;
  };

  //! LightSource
  class LightSource : public Object3D {
  public:
    LightSource();
    virtual void render(glm::mat4 M, GLuint program_ID);
    
    float intensity;
    glm::vec3 color;
    glm::vec3 position;
  private:
  };

  //! SimpleGraphicsEngine
  class SimpleGraphicsEngine {
  public:
    SimpleGraphicsEngine();
    virtual ~SimpleGraphicsEngine();
    
    void run();
  protected:
    virtual void update() = 0;
    virtual void render() = 0;
    
    // Probably should be private...
    static GLFWwindow* window_;
    
    double dt_;
    
    static Object3D* scene_;
    //Object3D* view_space_;
    //Object3D* background_space_;
    //static Object3D* camera_;
    //static Object3D* viewspace_ortho_camera_;
    
  private:
    bool initialize();
    
    double time_;
    
    //PerspectiveCamera* basic_cam_;
    //OrthoCamera* background_ortho_cam_;
  };

  class FBO
  {
  public:
    FBO(int width, int height, int int_method);
    ~FBO();

    static void useFBO(FBO *out, FBO *in1, FBO *in2);
    static void CHECK_FRAMEBUFFER_STATUS();

    GLuint texid_;
    GLuint fb_;
    GLuint rb_;
    GLuint depth_;
    int width_, height_;
  };

  class FBO3D
  {
  public:
    FBO3D(int size);
    ~FBO3D();

    static void useFBO(FBO3D *out, FBO3D *in1, FBO3D *in2);
    static void CHECK_FRAMEBUFFER_STATUS();

    GLuint texid_;
    GLuint fb_;
    GLuint rb_;
    GLuint depth_;
    int size_;
  };
}

#endif