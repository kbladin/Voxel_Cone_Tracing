#ifndef SHADER_WINDOW_H
#define SHADER_WINDOW_H

#include <vector>
#include <string>

#include <gl/glfw3.h>

//! Load and compile a shader program.
/*!
 Function referenced from www.opengl-tutorials.org.
*/
class ShaderLoader {
public:
  static GLuint loadShaders(
     const char * vertex_file_path,
     const char * tesselation_control_file_path,
     const char * tesselation_eval_file_path,
     const char * geometry_file_path,
     const char * fragment_file_path);
};

#endif