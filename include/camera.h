/**
* @file camera.h
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera clas
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <GL/glew.h>
#include "glm/glm.hpp"

#include "entity.h"

typedef struct cameraMatrices {
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

	//glm::mat4 getViewMatrix();
	//glm::mat4 getProjectionMatrix() { return matrices.projection; }
	//glm::mat4 getViewProjectionMatrix();

	void update(double dt);

	glm::vec3 mFacing;

private:
	void updateUniform();

	cameraMatrices_t matrices;
	uint32_t mWidth;
	uint32_t mHeight;
	float mFOV;

	GLuint mCameraUniformBuffer;
};

#endif
