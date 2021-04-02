/**
 * @file camera.h
 * @author Erik Sandrén
 * @date 20-12-2015
 * @brief Camera clas
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include <GL/glew.h>

#include "entity.h"
#include "frustum.h"
#include "mesh.h"

typedef struct CameraProperties {
	float FOV;
	float aspectRatio;
	float nearPlane;
	float farPlane;
} cameraProperties_t;

typedef struct CameraMatrices {
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 vp;
} cameraMatrices_t;

class Camera : public Entity {
public:
	Camera(const uint32_t width, const uint32_t height, const float fov);
	~Camera();

	void initialize();
	void setProjectionMatrix(glm::mat4 proj);

	// glm::mat4 getViewMatrix();
	// glm::mat4 getProjectionMatrix() { return matrices.projection; }
	// glm::mat4 getViewProjectionMatrix();

	void update(double dt);

	const glm::vec3& getFacing() const { return mFacing; }
	void setFacing(const glm::vec3 facing) { mFacing = facing; }

	const CameraProperties getProperties() const;

	Frustum getFrustum();

	void renderFrustum() const;

	// Debugging purpose
	bool frustumDetached() const { return mDetachFrustum; }
	void detachFrustum(bool status);

private:
	void updateUniform();

	CameraMatrices matrices;

	uint32_t mWidth;
	uint32_t mHeight;
	float mAspect;
	float mFOV;
	float mNearPlane;
	float mFarPlane;

	glm::vec3 mFacing;
	glm::vec3 mFrustumPosition;

	glm::mat4 mFrustumMatrix;

	GLuint mCameraUniformBuffer;

	Frustum mCameraFrustum;
	bool mDetachFrustum;
	Mesh mCameraFrustumMesh;
};

#endif
