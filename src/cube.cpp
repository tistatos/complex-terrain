/**
* @file cube.cpp
* @author Erik Sandrén
* @date 07-07-2018
* @brief simple cube model
*/
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "cube.h"
#include "mesh.h"
#include "shader.h"

Cube::Cube(Program& shader) : mShader(shader) {
	mMesh = Mesh::createCube();
}

void Cube::update(const double dt) {
	rotate(glm::vec3(0.0f, 0.001f*dt, 0.0f));
}
void Cube::render(const double dt) const {
	mShader.useProgram();
	glUniformMatrix4fv(glGetUniformLocation(mShader, "m"),
			1, GL_FALSE, glm::value_ptr(getModelMatrix()));
	mMesh.renderMesh();
}
