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
#include <iostream>

class Shader {
public:
  Shader(char* path, GLenum shaderType);
  ~Shader();
  GLuint getID() {return mShaderID; };
  void reloadShader();
private:
  void compileShader(const char* source);
  void getCompileErrors();
  char* readSource(char* path);
  GLuint mShaderID;
  char* mShaderPath;
};

class Program {
public:
  Program(std::string name);
  void attach(Shader* s);
  void useProgram() { glUseProgram(mProgramID); };
  void linkProgram();
  std::string getName() { return mProgramName; }
private:
  GLuint mProgramID;
  std::string mProgramName;
  void getLinkingError();
};


#endif
