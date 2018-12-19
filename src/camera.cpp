/**
* @file camera.cpp
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera class
*/

#include <glm/gtc/type_ptr.hpp>
#include <cstring> //for memcpy

#include "camera.h"
#include "engine.h"
#include "shader.h"
#include "shaderManager.h"

Camera::Camera(const uint32_t width, const uint32_t height, const float fov)
	: mWidth(width), mHeight(height), mAspect((float)mWidth/mHeight),
	mFOV(fov), mNearPlane(1.0f), mFarPlane(500.0f), mDetachFrustum(false) {
}

Camera::~Camera() {
	glDeleteBuffers(1, &mCameraUniformBuffer);
}

void Camera::initialize() {
	glGenBuffers(1, &mCameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrices), NULL, GL_STATIC_DRAW);

	setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	mFacing = glm::vec3(0.0f,0.0f,-1.0f);

	matrices.projection = glm::perspective(
				mFOV,
				mAspect,
				mNearPlane,
				mFarPlane );

	matrices.view = glm::lookAt(
			getPosition(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f,0.0f) );

	mCameraFrustum = Frustum(this);
	mCameraFrustumMesh = Mesh::createFrustum(&mCameraFrustum);

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

const CameraProperties Camera::getProperties() const {
	return CameraProperties {
		mFOV, mAspect, mNearPlane, mFarPlane
	};
}

void Camera::detachFrustum(bool status) {
	mDetachFrustum = status;
	mFrustumPosition = mPosition;
	mFrustumMatrix = glm::inverse(matrices.view);
	//mFrustumPosition.z -= mFarPlane / 2.0;
}

Frustum Camera::getFrustum() {
	if(!mDetachFrustum)
		mCameraFrustum = Frustum(this);
	return mCameraFrustum;
}

void Camera::renderFrustum() const {
	Program* bb = ShaderManager::getInstance()->getShader("bbox");
	bb->useProgram();

	glUniformMatrix4fv(glGetUniformLocation(*bb, "m"), 1, GL_FALSE, glm::value_ptr(mFrustumMatrix));
	mCameraFrustumMesh.renderLines();
}

void Camera::updateUniform() {
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);

	GLvoid* pointer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(pointer, &this->matrices, sizeof(matrices));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mCameraUniformBuffer, 0, sizeof(matrices));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
