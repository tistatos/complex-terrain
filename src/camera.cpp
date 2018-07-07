/**
* @file camera.cpp
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera class
*/
#include <cstring>

#include "camera.h"

Camera::Camera(const uint32_t width, const uint32_t height, const float fov)
	: mWidth(width), mHeight(height), mFOV(fov) { }

Camera::~Camera() {
	glDeleteBuffers(1, &mCameraUniformBuffer);
}

void Camera::initialize() {
	glGenBuffers(1, &mCameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraMatrices), NULL, GL_STATIC_DRAW);

	setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	mFacing = glm::vec3(0.0f,0.0f,-1.0f);

	float aspect = (float)mWidth/mHeight;
	matrices.projection = glm::perspective(
				mFOV,
				aspect,
				0.1f,
				100.0f);

	matrices.view = glm::lookAt(
			getPosition(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f,0.0f) );
}

void Camera::setProjectionMatrix(glm::mat4 proj) {
	matrices.projection = proj;
}

void Camera::update(double dt) {
	matrices.view = glm::lookAt(
			getPosition(),
			getPosition() + mFacing,
			glm::vec3(0.0f, 1.0f,0.0f) );

	matrices.vp = matrices.projection * matrices.view;
	updateUniform();
}

void Camera::updateUniform() {
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);

	GLvoid* pointer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(pointer, &this->matrices, sizeof(matrices));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mCameraUniformBuffer, 0, sizeof(matrices));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
