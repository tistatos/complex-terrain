/**
* @file engine.h
* @author Erik Sandrén
* @date 13-12-2015
* @brief complex terrain engine
*/
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>

#include "gui.h"
#include "camera.h"
#include "terrain.h"
#include "shaderManager.h"
#include "gameTimer.h"

class GUI;

class CTEngine
{
public:
	CTEngine(char* name);
	~CTEngine();
	bool initialize();
	void shutdown();

	void update(double dt);
	void render(double dt);
	void run();

	bool running() { return mRunning; }

private:
	Terrain mTerrain;
	Camera mCamera;
	GameTimer mTimer;
	GUI* mGUI;

	bool mRunning;

	float mFov;
	float mMovementSpeed;
	float mLookSpeed;

	float mPreviousMouseX;
	float mPreviousMouseY;

	const GLFWvidmode* vidmode;
	GLFWwindow* mWindow;
	GLFWmonitor* mMonitor;

	char mTitle[64];
};

#endif
