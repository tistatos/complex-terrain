/**
* @file vertex.h
* @author Erik Sandrén
* @date 07-07-2018
* @brief simple vertex with only position and color
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
};


#endif
