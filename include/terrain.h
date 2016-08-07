/**
* @file terrain.h
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "shader.h"

class Terrain {
public:
	Terrain();
	void initialize();
	void render();
	void update();

private:
	Program mDensityProgram;
};


#endif
