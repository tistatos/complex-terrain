/**
 * @file frustum.cpp
 * @author Erik Sandrén
 * @date 16-09-2018
 * @brief Camera frustum
 */

#include "frustum.h"
#include "AABBox.h"
#include "camera.h"
#include "engine.h" // for up vector

#include <glm/gtx/string_cast.hpp>
#include <iostream>

Frustum::Frustum(const Camera* c) {
	const CameraProperties camProp = c->getProperties();
	const float& fov = camProp.FOV;
	const float& farPlane = camProp.farPlane;
	const float& nearPlane = camProp.nearPlane;
	const float& aspect = camProp.aspectRatio;

	// These are half the width and height!
	// FIXME: this should be the correct value
	// however its not the case and all width and heights get too small
	// const float tanFov = tan((fov * M_PI / 180.0) * 1.00);
	const float tanFov = tan(fov * 0.50);

	const float halfhNear = tanFov * nearPlane;
	const float halfwNear = halfhNear * aspect;
	const float halfhFar = tanFov * farPlane;
	const float halfwFar = halfhFar * aspect;

	const glm::vec3 p = c->getPosition();
	const glm::vec3 d = c->getFacing();
	glm::vec3 up = CTEngine::getUpVector();
	const glm::vec3 right = glm::normalize(glm::cross(d, up));
	up = glm::normalize(glm::cross(right, d));

	const glm::vec3 fc = p + d * farPlane;
	const glm::vec3 nc = p + d * nearPlane;

	const glm::vec3 halfwNearVec = right * glm::vec3(halfwNear);
	const glm::vec3 halfhNearVec = up * glm::vec3(halfhNear);

	const glm::vec3 halfwFarVec = right * glm::vec3(halfwFar);
	const glm::vec3 halfhFarVec = up * glm::vec3(halfhFar);

	mPoints[0] = nc - halfwNearVec - halfhNearVec;
	mPoints[1] = nc - halfwNearVec + halfhNearVec;
	mPoints[2] = nc + halfwNearVec - halfhNearVec;
	mPoints[3] = nc + halfwNearVec + halfhNearVec;

	mPoints[4] = fc - halfwFarVec - halfhFarVec;
	mPoints[5] = fc - halfwFarVec + halfhFarVec;
	mPoints[6] = fc + halfwFarVec - halfhFarVec;
	mPoints[7] = fc + halfwFarVec + halfhFarVec;

	const glm::vec3 topNormal = glm::cross(glm::normalize((nc + up * halfhNear) - p), right);
	const glm::vec3 bottomNormal = glm::cross(right, glm::normalize((nc - up * halfhNear) - p));
	const glm::vec3 leftNormal = glm::cross(glm::normalize((nc - right * halfwNear) - p), up);
	const glm::vec3 rightNormal = glm::cross(up, glm::normalize((nc + right * halfwNear) - p));

	mPlanes[TOP] = createPlane(topNormal, nc + up * halfhNear);
	mPlanes[BOTTOM] = createPlane(bottomNormal, nc - up * halfhNear);
	mPlanes[LEFT] = createPlane(leftNormal, nc - right * halfhNear);
	mPlanes[RIGHT] = createPlane(rightNormal, nc + right * halfhNear);
	mPlanes[NEAR] = createPlane(d, nc);
	mPlanes[FAR] = createPlane(-d, fc);
}

const glm::vec4 Frustum::createPlane(const glm::vec3& normal, const glm::vec3& point) const {
	const float d = -(glm::dot(normal, point));
	return glm::vec4(normal, d);
}

const bool Frustum::intersects(const AABBox& bbox) const {
	const glm::vec3& bbmax = bbox.getMax();
	const glm::vec3& bbmin = bbox.getMin();

	// Bounding box intersects if all 8 points are outside frustum
	for (unsigned int i = 0; i < FRUSTUM_PLANE_COUNT; ++i) {
		unsigned int out = 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmin, 1.0f)) < 0.0 ? 1 : 0;

		out += glm::dot(mPlanes[i], glm::vec4(bbmin.x, bbmin.y, bbmax.z, 1.0f)) < 0.0 ? 1 : 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmin.x, bbmax.y, bbmin.z, 1.0f)) < 0.0 ? 1 : 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmin.x, bbmax.y, bbmax.z, 1.0f)) < 0.0 ? 1 : 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmax.x, bbmin.y, bbmin.z, 1.0f)) < 0.0 ? 1 : 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmax.x, bbmin.y, bbmax.z, 1.0f)) < 0.0 ? 1 : 0;
		out += glm::dot(mPlanes[i], glm::vec4(bbmax.x, bbmax.y, bbmin.z, 1.0f)) < 0.0 ? 1 : 0;

		out += glm::dot(mPlanes[i], glm::vec4(bbmax, 1.0f)) < 0.0 ? 1 : 0;
		if (out == 8) {
			return false;
		}
	}

	return true;
}

const glm::vec3* Frustum::getFrustumPoints() const { return mPoints; }
