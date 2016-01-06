/**
* @file shaderManager.cpp
* @author Erik Sandrén
* @date 28-12-2015
* @brief shader manager class
*/

#include "shaderManager.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::addShader(Program* s)
{
  mShaders.push_back(s);
}

Program* ShaderManager::getShader(std::string name)
{
  std::vector<Program*>::iterator iter = mShaders.begin();
  for(;iter != mShaders.end(); ++iter)
  {
    if((*iter)->getName() == name)
      return (*iter);
  }
}

