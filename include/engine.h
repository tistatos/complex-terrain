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

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class GUI;
class Cube;
class Camera;

struct FPSCounter {
	double timePassed;
	double latestCount;
	int frameCount;

	FPSCounter() : timePassed(0), frameCount(0), latestCount(0) { }

	void calculateFPS(double dt) {
		timePassed += dt;
		frameCount++;
		if(timePassed > 1000.0) {
			latestCount = (double)frameCount / (timePassed / 1000.0);
			timePassed = 0.0;
			frameCount = 0;
		}
	}
};

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
	FPSCounter mFPSCounter;

	bool mRunning;
	bool mCaptureCursor;

	float mMovementSpeed;
	float mLookSpeed;

	double mPreviousMouseX;
	double mPreviousMouseY;

	const GLFWvidmode* vidmode;
	GLFWwindow* mWindow;
	GLFWmonitor* mMonitor;

	uint32_t mWidth;
	uint32_t mHeight;

	char mTitle[64];

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
};

#endif
