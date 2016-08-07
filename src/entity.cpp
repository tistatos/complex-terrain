/**
* @file entity.cpp
* @author Erik Sandrén
* @date 29-07-2016
* @brief Entity within the CTEngine
*/

#include "entity.h"

void Entity::translate(glm::vec3 trans) {
	mModelMatrix = glm::translate(mModelMatrix, trans);
}

void Entity::rotate(glm::mat4 rot) {
	mModelMatrix *= rot;
}
