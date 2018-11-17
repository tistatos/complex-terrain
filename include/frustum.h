/**
* @file frustum.h
* @author Erik Sandrén
* @date 16-09-2018
* @brief [Description Goes Here]
*/

#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "glm/glm.hpp"

#define FRUSTUM_PLANE_COUNT 6
#define FRUSTUM_POINT_COUNT 8

class Camera;
class AABBox;

class Frustum {
private:
	//plane index enum
	enum { TOP = 0, BOTTOM, LEFT, RIGHT, NEAR, FAR };
	glm::vec4 mPlanes[FRUSTUM_PLANE_COUNT];
	glm::vec3 mPoints[FRUSTUM_POINT_COUNT];

	//FIXME: how do we avoid not sending parameters in the wrong order? assert that normal is normalized?
	const glm::vec4 createPlane(const glm::vec3& normal, const glm::vec3& point) const;

public:
	Frustum() { }
	Frustum(const Camera* c);
	const bool intersects(const AABBox& boundingBox) const;
	const glm::vec3* getFrustumPoints() const;
	//bool intersects(const glm::vec3& point) const;
	//bool intersects(const glm::vec3& sperePoint, const float& radius) const;
};


#endif
