/**
* @file entity.h
* @author Erik Sandrén
* @date 29-07-2016
* @brief Entity within the CTEngine
*/

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Entity {
public:
	glm::mat4 getModelMatrix() { return mModelMatrix; }
	glm::vec3 getPosition() { return glm::vec3(getModelMatrix()[3]); }

	void translate(glm::vec3 trans);
	void rotate(glm::mat4 rot);

protected:
	glm::mat4 mModelMatrix;
};



#endif
