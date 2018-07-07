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

//TODO: avoid reading file here since we can crash if the file is not available
Shader::Shader(const char* path, GLenum shaderType) {
	mShaderID = glCreateShader(shaderType);
	mShaderPath = path;
	const char* source =  readSource(path);
	compileShader(source);
	getCompileErrors();
}

void Shader::compileShader(const char* source) {
	glShaderSource(mShaderID, 1, &source, nullptr);
	glCompileShader(mShaderID);
}

void Shader::reloadShader() {
	const char* source = readSource(mShaderPath);
	compileShader(source);
	getCompileErrors();
}

Shader::~Shader() {
	glDeleteShader(mShaderID);
}

const char* Shader::readSource(const char* filename) {
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		std::cout << "error finding file: " << filename << std::endl;
		return 0;
	}
	int bytesinfile = filelength(file);
	char *buffer = (char*)malloc(bytesinfile+1);
	int bytesread = fread( buffer, 1, bytesinfile, file);
	buffer[bytesread] = 0; // Terminate the string with 0
	fclose(file);
	return buffer;
}

void Shader::getCompileErrors() {
	GLint status = 0;
	glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
		return;
	char log[4096];
	glGetShaderInfoLog(mShaderID, sizeof(log), nullptr, log);
	std::cerr << "Shader Compile error - " << mShaderPath << ":" << std::endl;
	std::cerr << log << std::endl;
}

/**
 * Shader program
 **/

Program::Program(std::string name) {
	mProgramID = 0;
	mProgramName = name;
}

Program::~Program() {
	std::vector<Shader*>::iterator iter = mAttachedShaders.begin();
	for(; iter != mAttachedShaders.end(); ++iter) {
		delete (*iter);
	}
}

void Program::linkProgram() {
	glLinkProgram(mProgramID);
	getLinkingError();
}

void Program::getLinkingError() {
	GLint status = 0;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
	if(status == GL_TRUE)
		return;

	char log[4096];
	glGetProgramInfoLog(mProgramID, sizeof(log), nullptr, log);
	std::cerr << "Program Link error - " << mProgramName << ":" << std::endl;
	std::cerr << log << std::endl;
}

void Program::attach(Shader* s) {
	if(mProgramID == 0)
		mProgramID = glCreateProgram();
	glAttachShader(mProgramID, s->getID());
	mAttachedShaders.push_back(s);
}

void Program::reload() {
	std::vector<Shader*>::iterator iter = mAttachedShaders.begin();
	for(;iter != mAttachedShaders.end(); ++iter)
	{
		(*iter)->reloadShader();
		linkProgram();
	}
}
