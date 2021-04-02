/**
 * @file shaderManager.h
 * @author Erik Sandrén
 * @date 28-12-2015
 * @brief Global shader manager
 */

#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include <string>
#include <vector>
class Program;

class ShaderManager {
public:
	void addShader(Program* s);
	Program* getShader(std::string name);
	void updateShaders();
	void disposeShaders();
	static ShaderManager* getInstance();

private:
	static ShaderManager* mInstance;

	ShaderManager(){};
	std::vector<Program*> mShaders;
};
#endif
