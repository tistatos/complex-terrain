/**
* @file engine.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief Complex terrain engine
*/

#include "engine.h"
#include "shader.h"
CTEngine::CTEngine(char* name)
	: mFov(45.0f)
	, mMovementSpeed(10.0f)
	, mLookSpeed(10.0f)
	{
  mRunning = false;
  sprintf(mTitle, name);
}

CTEngine::~CTEngine() { }

bool CTEngine::initialize() {
  if(!glfwInit())
    return false;

  mMonitor = glfwGetPrimaryMonitor();
  vidmode = glfwGetVideoMode(mMonitor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  mWindow = glfwCreateWindow(vidmode->width/2, vidmode->height/2, mTitle, NULL, NULL);

  if (!mWindow) {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(0);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glewInit();

  mCamera.setProjectionMatrix(
			glm::perspective(
        mFov,
        (float)(vidmode->width/vidmode->height),
        0.1f,
        100.0f
			)
  );

	mTerrain.initialize();

  mRunning = true;
  return true;
}

void CTEngine::run() {
	double dt;
	while(running()) {
		mTimer.tick();
		dt = mTimer.getDeltaTime();

		update(dt);
		render(dt);
	}
}

void CTEngine::update(double dt) {
  glfwPollEvents();
  char title[64];
  sprintf(title,"%s, %.4f ms", mTitle, dt);
  glfwSetWindowTitle(mWindow, title);
  if(glfwGetKey(mWindow, GLFW_KEY_ESCAPE)) {
    mRunning = false;
    glfwSetWindowShouldClose(mWindow, GL_TRUE);
  }


	double mouseX, mouseY;
	glfwGetCursorPos(mWindow, &mouseX, &mouseY);

	double mouseDeltaX = mouseX - mPreviousMouseX;
	double mouseDeltaY = mouseY - mPreviousMouseY;

	mPreviousMouseX = mouseX;
	mPreviousMouseY = mouseY;

  glm::vec3 movement;

  if(glfwGetKey(mWindow, GLFW_KEY_W))
    movement.z += mMovementSpeed*dt;
  if(glfwGetKey(mWindow, GLFW_KEY_A))
    movement.z -= mMovementSpeed*dt;
  if(glfwGetKey(mWindow, GLFW_KEY_S))
    movement.x += mMovementSpeed*dt;
  if(glfwGetKey(mWindow, GLFW_KEY_D))
    movement.x -= mMovementSpeed*dt;
  if(glfwGetKey(mWindow, GLFW_KEY_SPACE))
    movement.y += mMovementSpeed*dt;
  if(glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL))
    movement.y -= mMovementSpeed*dt;

	mCamera.translate(movement);
}

void CTEngine::render(double dt) {
  glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown() {
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}
