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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

class Shader {
public:
	Shader(const char* path, GLenum shaderType);
	~Shader();
	GLuint getID() { return mShaderID; };
	void reloadShader();

private:
	void compileShader(const char* source);
	void getCompileErrors();
	const char* readSource(const char* path);
	GLuint mShaderID;
	const char* mShaderPath;
};

class Program {
public:
	Program(std::string name);
	~Program();

	void attach(Shader* s);
	void useProgram() const { glUseProgram(mProgramID); };
	void linkProgram();
	void reload();

	GLuint getProgram() { return mProgramID; }
	std::string getName() { return mProgramName; }

	operator GLuint() { return mProgramID; }

private:
	GLuint mProgramID;
	std::string mProgramName;
	void getLinkingError();
	std::vector<Shader*> mAttachedShaders;
};

#endif
