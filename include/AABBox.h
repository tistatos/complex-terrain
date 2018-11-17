/**
* @file AABBox.h
* @author Erik Sandrén
* @date 16-09-2018
* @brief [Description Goes Here]
*/

#ifndef __AABBOX_H__
#define __AABBOX_H__

#include<glm/vec3.hpp>

class AABBox
{
private:
	glm::vec3 mMax;
	glm::vec3 mMin;

public:
	AABBox(glm::vec3 maxPoint, glm::vec3 minPoint) : mMax(maxPoint), mMin(minPoint) { }
	const glm::vec3& getMax() const { return mMax; }
	const glm::vec3& getMin() const { return mMin; }

	void setMax(const glm::vec3& max) { mMax = max;}
	void setMin(const glm::vec3& min) { mMin = min;}
};

#endif
