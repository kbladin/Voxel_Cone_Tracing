#include "../include/ShaderLoader.h"
#include <fstream>

#include <GL/glew.h>
//#include <GLFW/glfw3.h>


// https://www.omniref.com/ruby/gems/opengl-bindings/1.3.5/symbols/OpenGL::GL_TESS_CONTROL_SHADER

#ifndef GL_TESS_CONTROL_SHADER
    #define GL_TESS_CONTROL_SHADER 0x8E88
#endif
#ifndef GL_TESS_EVALUATION_SHADER
    #define GL_TESS_EVALUATION_SHADER 0x8E87
#endif
#ifndef GL_GEOMETRY_SHADER
    #define GL_GEOMETRY_SHADER 0x8DD9
#endif
#ifndef GL_PATCHES
    #define GL_PATCHES 0x000E
#endif

GLuint ShaderLoader::loadShaders(
  const char * vertex_file_path,
  const char * tesselation_control_file_path,
  const char * tesselation_eval_file_path,
  const char * geometry_file_path,
  const char * fragment_file_path)
{
  // Create the shaders
  // Vertex and fragment shader is a must
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  
  GLuint TesselationControlShaderID;
  GLuint TesselationEvaluationShaderID;
  GLuint GeometryShaderID;

  if (tesselation_control_file_path)
    TesselationControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
  if (tesselation_eval_file_path)
    TesselationEvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
  if (geometry_file_path)
    GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open())
  {
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }
  
  std::string TesselationControlShaderCode;
  if (tesselation_control_file_path){
    // Read the Tesselation Control Shader code from the file
    std::ifstream TesselationControlShaderStream(tesselation_control_file_path, std::ios::in);
    if(TesselationControlShaderStream.is_open()){
      std::string Line = "";
      while(getline(TesselationControlShaderStream, Line))
        TesselationControlShaderCode += "\n" + Line;
      TesselationControlShaderStream.close();
    }
  }

  std::string TesselationEvaluationShaderCode;
  if (tesselation_eval_file_path){
    // Read the Tesselation Evaluation Shader code from the file
    std::ifstream TesselationEvalStream(tesselation_eval_file_path, std::ios::in);
    if(TesselationEvalStream.is_open()){
      std::string Line = "";
      while(getline(TesselationEvalStream, Line))
        TesselationEvaluationShaderCode += "\n" + Line;
      TesselationEvalStream.close();
    }
  }

  std::string GeometryShaderCode;
  if (geometry_file_path){
    // Read the Geometry Shader code from the file
    std::ifstream GeomatryShaderStream(geometry_file_path, std::ios::in);
    if(GeomatryShaderStream.is_open()){
      std::string Line = "";
      while(getline(GeomatryShaderStream, Line))
        GeometryShaderCode += "\n" + Line;
      GeomatryShaderStream.close();
    }
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }
  
  GLint Result = GL_FALSE;
  int InfoLogLength;
  
  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
  
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> VertexShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
  


  if (tesselation_control_file_path){
    // Compile Tesselation Control Shader
    printf("Compiling shader : %s\n", tesselation_control_file_path);
    char const * TesselationControlSourcePointer = TesselationControlShaderCode.c_str();
    glShaderSource(TesselationControlShaderID, 1, &TesselationControlSourcePointer , NULL);
    glCompileShader(TesselationControlShaderID);
    
    // Check Tesselation Control Shader
    glGetShaderiv(TesselationControlShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(TesselationControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> TesselationControlShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(TesselationControlShaderID, InfoLogLength, NULL, &TesselationControlShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &TesselationControlShaderErrorMessage[0]);
  }

  if (tesselation_eval_file_path){
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", tesselation_eval_file_path);
    char const * TesselationEvaluationSourcePointer = TesselationEvaluationShaderCode.c_str();
    glShaderSource(TesselationEvaluationShaderID, 1, &TesselationEvaluationSourcePointer , NULL);
    glCompileShader(TesselationEvaluationShaderID);
    
    // Check TesselationEvaluation Shader
    glGetShaderiv(TesselationEvaluationShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(TesselationEvaluationShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> TesselationEvaluationShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(TesselationEvaluationShaderID, InfoLogLength, NULL, &TesselationEvaluationShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &TesselationEvaluationShaderErrorMessage[0]);
  }

  if (geometry_file_path){
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", geometry_file_path);
    char const * GeometrySourcePointer = GeometryShaderCode.c_str();
    glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer , NULL);
    glCompileShader(GeometryShaderID);
    
    // Check Geometry Shader
    glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> GeometryShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &GeometryShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
  
  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
  
  // Link the program
  fprintf(stdout, "Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  if (tesselation_control_file_path)
    glAttachShader(ProgramID, TesselationControlShaderID);
  if (tesselation_eval_file_path)
    glAttachShader(ProgramID, TesselationEvaluationShaderID);
  if (geometry_file_path)
    glAttachShader(ProgramID, GeometryShaderID);
  glAttachShader(ProgramID, FragmentShaderID);

  glLinkProgram(ProgramID);
  
  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
  glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
  
  glDeleteShader(VertexShaderID);
  if (tesselation_control_file_path)
    glDeleteShader(TesselationControlShaderID);
  if (tesselation_eval_file_path)
    glDeleteShader(TesselationEvaluationShaderID);
  if (geometry_file_path)
    glDeleteShader(GeometryShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}