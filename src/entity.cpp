/**
* @file entity.cpp
* @author Erik Sandrén
* @date 29-07-2016
* @brief Entity within the CTEngine
*/

#include "entity.h"
Entity::Entity() : mScale(1.0, 1.0, 1.0) {
}

const glm::mat4 Entity::getModelMatrix() const {
	glm::mat4 m = glm::translate(glm::yawPitchRoll(mRotation.y, mRotation.x, mRotation.z)*glm::scale(glm::mat4(), mScale), mPosition);
		return m;

}
void Entity::translate(glm::vec3 trans) {
	mPosition += trans;
}

void Entity::rotate(glm::vec3 rot) {
	mRotation += rot;
}
