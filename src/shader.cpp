/**
* @file shader.cpp
* @author Erik Sandrén
* @date 21-12-2015
* @brief Shader Class
*/

#include "shader.h"
#include <iostream>
long filelength(FILE *file) {
    long numbytes;
    long savedpos = ftell(file); // Remember where we are
    fseek(file, 0, SEEK_END);    // Fast forward to the end
    numbytes = ftell(file);      // Index of last byte in file
    fseek(file, savedpos, SEEK_SET); // Get back to where we were

    return numbytes;             // This is the file length
}

Shader::Shader(char* path, GLenum shaderType)
{
  mShaderID = glCreateShader(shaderType);
  const char* source =  readSource(path);
  glShaderSource(mShaderID, 1, &source, nullptr);

  glCompileShader(mShaderID);
  getCompileErrors();
}

Shader::~Shader()
{
  glDeleteShader(mShaderID);
}

char* Shader::readSource(char* filename)
{
  FILE *file = fopen(filename, "r");
  if(file == NULL)
  {
    std::cout << "error founding file" << std::endl;
    return 0;
  }
  int bytesinfile = filelength(file);
  char *buffer = (char*)malloc(bytesinfile+1);
  int bytesread = fread( buffer, 1, bytesinfile, file);
  buffer[bytesread] = 0; // Terminate the string with 0
  fclose(file);
  return buffer;
}

void Shader::getCompileErrors()
{
  GLint status = 0;
  glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &status);
  if (status == GL_TRUE)
    return;
  char log[4096];
  glGetShaderInfoLog(mShaderID, sizeof(log), nullptr, log);
  std::cerr << "Shader Compile error:" << std::endl;
  std::cerr << log << std::endl;
}

Program::Program()
{
  mProgramID = glCreateProgram();
}

void Program::linkProgram()
{
  glLinkProgram(mProgramID);
  getLinkingError();
}

void Program::getLinkingError()
{
  GLint status = 0;
  glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
  if(status == GL_TRUE)
    return;

  char log[4096];
  glGetProgramInfoLog(mProgramID, sizeof(log), nullptr, log);
  std::cerr << "Progam Link error:" << std::endl;
  std::cerr << log << std::endl;

}
