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
#include "glm/gtx/euler_angles.hpp"

class Entity {
public:
	Entity();
	const glm::mat4 getModelMatrix() const;
	const glm::vec3& getPosition() const { return mPosition; }
	const glm::vec3& getRotation() const { return mRotation; }

	void setPosition(const glm::vec3& position) { mPosition = position; }
	void setRotation(const glm::vec3& rotation) { mRotation = rotation; }

	void translate(glm::vec3 trans);
	void rotate(glm::vec3 rot);

protected:
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;
};

#endif
