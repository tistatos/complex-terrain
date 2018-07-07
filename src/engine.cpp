/**
* @file engine.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief Complex terrain engine
*/
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "engine.h"
#include "shaderManager.h"
#include "gui.h"
#include "cube.h"
#include "entity.h"
#include "camera.h"
#include "shader.h"

static const GLfloat vertexDataArray[] =
{
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

static const GLfloat colorDataArray[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};
//Index array
static const GLuint indexArrayData[] = { 0,1,2 };

CTEngine::CTEngine(const char* name)
	: mMovementSpeed(1.0f)
	, mLookSpeed(10.0f)
	{
	mRunning = false;
	sprintf(mTitle, "%s", name);
}

bool CTEngine::initialize() {
	//Initialize glfw
	if(!glfwInit())
		return false;

	//Create window
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

	//Initialize glew
	glewExperimental = GL_TRUE;
	glewInit();
	glfwGetCursorPos(mWindow, &mPreviousMouseX, &mPreviousMouseY);

	//Setup camera
	mCamera = new Camera(vidmode->width, vidmode->height, 45.0f);
	mCamera->initialize();

	//Create gui
	mGUI = new GUI();
	if(!mGUI->initialize()) {
		return false;
	}

	//initalize Other stuff,
	Shader* cubeVertexShader = new Shader("../shaders/cube.vert", GL_VERTEX_SHADER);
	Shader* cubeFragmentShader  = new Shader("../shaders/cube.frag", GL_FRAGMENT_SHADER);
	Program* cubeProgram = new Program("simpleCube");
	cubeProgram->attach(cubeVertexShader);
	cubeProgram->attach(cubeFragmentShader);
	cubeProgram->linkProgram();
	ShaderManager::getInstance()->addShader(cubeProgram);

	Shader* colorV = new Shader("../shaders/color.vert", GL_VERTEX_SHADER);
	Shader* colorF  = new Shader("../shaders/color.frag", GL_FRAGMENT_SHADER);
	Program* color  = new Program("color");
	color->attach(colorV);
	color->attach(colorF);
	color->linkProgram();
	ShaderManager::getInstance()->addShader(color);

	mCube = new Cube(*cubeProgram);

	glGenVertexArrays(1, &vertexArrayID);

	//Generate buffers and store identifiers
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &indexBufferID);
	glGenBuffers(1, &colorBufferID);

	//Bind vertexArray
	glBindVertexArray(vertexArrayID);
	//bind vertex buffer and put our data into the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexDataArray), vertexDataArray, GL_STATIC_DRAW);

	//Tell shaders what data is in this attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0,(void*)0);

	//Bind index buffer and insert our indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArrayData), indexArrayData, GL_STATIC_DRAW);

	//Bind color buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorDataArray), colorDataArray, GL_STATIC_DRAW);

	//Tell shaders what data is in this attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

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

	if(glfwGetKey(mWindow, GLFW_KEY_F5))
		ShaderManager::getInstance()->updateShaders();

	double mouseX, mouseY;
	glfwGetCursorPos(mWindow, &mouseX, &mouseY);

	double mouseDeltaX = mouseX - mPreviousMouseX;
	double mouseDeltaY = mouseY - mPreviousMouseY;

	mPreviousMouseX = mouseX;
	mPreviousMouseY = mouseY;

	float cameraSpeed = 0.01f;
	if(glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT))
		cameraSpeed += 0.1f;
	if(glfwGetKey(mWindow,GLFW_KEY_W))
		mCamera->translate(cameraSpeed * mCamera->mFacing);
	if(glfwGetKey(mWindow,GLFW_KEY_S))
		mCamera->translate(-cameraSpeed * mCamera->mFacing);
	if(glfwGetKey(mWindow,GLFW_KEY_A))
		mCamera->translate(-glm::normalize(glm::cross(mCamera->mFacing, glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);
	if(glfwGetKey(mWindow,GLFW_KEY_D))
		mCamera->translate(glm::normalize(glm::cross(mCamera->mFacing, glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);

	float rotatespeed = 0.005f;
	mCamera->mFacing = glm::rotateY(mCamera->mFacing, -(float)mouseDeltaX * rotatespeed);
	mCamera->mFacing.y -= rotatespeed * mouseDeltaY;
	glm::normalize(mCamera->mFacing);


	mCamera->update(dt);
	mCube->update(dt);

	mTerrain.update();
}

void CTEngine::render(double dt) {
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	mCube->render(dt);

	glm::vec3 camPos = mCamera->getPosition();
	glm::vec3 camRot = mCamera->mFacing;
	char camPosText[64];
	char camRotText[64];
	sprintf(camPosText,"x: %.3f, y: %.3f, z: %.3f", camPos.x, camPos.y, camPos.z);
	sprintf(camRotText,"x: %.3f, y: %.3f, z: %.3f", camRot.x, camRot.y, camRot.z);
	mGUI->renderText("Camera:", 10.0f, vidmode->height-20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	mGUI->renderText(camPosText, 10.0f, vidmode->height-40.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	mGUI->renderText(camRotText, 10.0f, vidmode->height-60.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));


	mTerrain.render();
	glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown() {
	ShaderManager::getInstance()->disposeShaders();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
