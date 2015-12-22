/**
* @file shader.h
* @author Erik Sandrén
* @date 20-12-2015
* @brief Shader class
*/

#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

class Shader {
public:
  Shader(char* path, GLenum shaderType);
  ~Shader();
  GLuint getID() {return mShaderID; };
private:
  void getCompileErrors();
  char* readSource(char* path);
  GLuint mShaderID;
};

class Program {
public:
  Program();
  void attach(Shader s) { glAttachShader(mProgramID, s.getID()); };
  void useProgram() { glUseProgram(mProgramID); };
  void linkProgram();
private:
  GLuint mProgramID;
  void getLinkingError();
};


#endif

