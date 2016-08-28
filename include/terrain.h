/**
* @file terrain.h
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "shader.h"
#include "shaderManager.h"

class Terrain {
public:
	void initialize();
	void render();
	void update();
};


#endif
