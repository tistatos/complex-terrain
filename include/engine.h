/**
* @file engine.h
* @author Erik Sandrén
* @date 13-12-2015
* @brief complex terrain engine
*/
#ifndef __ENGINE_H__
#define __ENGINE_H__

//#include <stdio.h>
//#include <stdlib.h>
//#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "terrain.h"
#include "gameTimer.h"

class GUI;
class Cube;
class Camera;

class CTEngine {
public:
	CTEngine(const char* name);
	bool initialize();
	void shutdown();

	void update(double dt);
	void render(double dt);
	void run();

	bool running() { return mRunning; }

private:
	Camera* mCamera;
	Terrain mTerrain;
	GameTimer mTimer;
	GUI* mGUI;
	Cube* mCube;

	bool mRunning;

	float mMovementSpeed;
	float mLookSpeed;

	double mPreviousMouseX;
	double mPreviousMouseY;

	const GLFWvidmode* vidmode;
	GLFWwindow* mWindow;
	GLFWmonitor* mMonitor;

	char mTitle[64];

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
};

#endif
