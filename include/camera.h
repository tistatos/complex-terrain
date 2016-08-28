/**
* @file camera.h
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera clas
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "entity.h"

class Camera : public Entity {
public:
	Camera();
	~Camera();
	void setProjectionMatrix(glm::mat4 proj) { mProjectionMatrix = proj; }

	glm::mat4 getProjectionMatrix() { return mProjectionMatrix; }
	glm::mat4 getViewMatrix() { return mModelMatrix; }
	glm::mat4 getViewProjectionMatrix() { return mProjectionMatrix * mModelMatrix; }


private:
	glm::mat4 mProjectionMatrix;
};

#endif
