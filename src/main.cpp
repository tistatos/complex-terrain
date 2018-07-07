/**
* @file main.cpp
* @author Erik Sandrén
* @date 12-12-2015
* @brief [Description Goes Here]
*/

#include <GL/glew.h>
#include <GL/gl.h>

#include "engine.h"

int main(int argc, char* argv[])
{
	CTEngine engine("Complex Terrain");
	if(!engine.initialize()) return -1;

	engine.run();

	engine.shutdown();
	return 0;
}
