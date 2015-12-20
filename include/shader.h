/**
* @file shader.h
* @author Erik Sandrén
* @date 20-12-2015
* @brief Shader class
*/

#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/gl.h>

class Shader {
public:
  Shader(char* path, GLenum shaderType);
  ~Shader();
  GLuint getID() {return mShaderID; };
private:
  GLuint mShaderID;
  GLenum mShaderType;
};


#endif

