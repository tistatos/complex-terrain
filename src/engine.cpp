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
	, mMovementSpeed(1.0f)
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

	glewExperimental = GL_TRUE;
	glewInit();

	mCamera.setProjectionMatrix(
			glm::perspective(
				mFov,
				(float)(vidmode->width/vidmode->height),
				0.1f,
				100.0f
			)
	);

	mGUI = new GUI();
	if(!mGUI->initialize())
		return false;

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
		movement.z += mMovementSpeed*dt/1000.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_S))
		movement.z -= mMovementSpeed*dt/1000.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_A))
		movement.x += mMovementSpeed*dt/1000.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_D))
		movement.x -= mMovementSpeed*dt/1000.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT))
		movement.y += mMovementSpeed*dt/1000.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL))
		movement.y -= mMovementSpeed*dt/1000.0f;

	if(glfwGetKey(mWindow, GLFW_KEY_F5))
			ShaderManager::getInstance()->updateShaders();
	mCamera.translate(movement);
}

void CTEngine::render(double dt) {
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 camPos = mCamera.getPosition();
	char camPosText[64];
	mGUI->renderText("Camera:", 10.0f, vidmode->height-20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	sprintf(camPosText,"x: %.3f, y: %.3f, z: %.3f", camPos.x, camPos.y, camPos.z);
	mGUI->renderText(camPosText, 10.0f, vidmode->height-40.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown() {
	ShaderManager::getInstance()->disposeShaders();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
