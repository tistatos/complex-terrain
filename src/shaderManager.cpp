/**
* @file shaderManager.cpp
* @author Erik Sandrén
* @date 28-12-2015
* @brief shader manager class
*/

#include "shaderManager.h"
#include "shader.h"

ShaderManager* ShaderManager::mInstance = 0;


ShaderManager* ShaderManager::getInstance() {
	if(!mInstance)
		ShaderManager::mInstance = new ShaderManager();
	return mInstance;
}

void ShaderManager::addShader(Program* s) {
	mShaders.push_back(s);
}

Program* ShaderManager::getShader(std::string name) {
	std::vector<Program*>::iterator iter = mShaders.begin();
	for(;iter != mShaders.end(); ++iter) {
		if((*iter)->getName() == name)
			return (*iter);
	}

	std::cout << "ERROR: Shader " << name << " not found" << std::endl;
	return nullptr;
}

void ShaderManager::updateShaders() {
	std::vector<Program*>::iterator iter = mShaders.begin();
	for(;iter != mShaders.end(); ++iter) {
		(*iter)->reload();
	}
}

void ShaderManager::disposeShaders() {
	std::vector<Program*>::iterator iter = mShaders.begin();
	for(;iter != mShaders.end(); ++iter) {
		delete (*iter);
	}
	mShaders.clear();
}
