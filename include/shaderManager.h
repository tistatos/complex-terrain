/**
* @file shaderManager.h
* @author Erik Sandrén
* @date 28-12-2015
* @brief [Description Goes Here]
*/

#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include "shader.h"
#include <vector>
class ShaderManager
{
public:
  void addShader(Program* s);
  Program* getShader(std::string name);
  ShaderManager();
  ~ShaderManager();

private:
  ShaderManager* mInstance;
  std::vector<Program*> mShaders;
};
#endif

