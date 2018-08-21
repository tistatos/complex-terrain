/**
* @file engine.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief Complex terrain engine
*/
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

#include "engine.h"
#include "shaderManager.h"
#include "gui.h"
#include "cube.h"
#include "entity.h"
#include "camera.h"
#include "shader.h"

class OpenGLDebugger {
public:
static void GLAPIENTRY
	MessageCallback( GLenum source,
									 GLenum type,
									 GLuint id,
									 GLenum severity,
									 GLsizei length,
									 const GLchar* message,
									 const void* userParam )
	{
		if(type == GL_DEBUG_TYPE_ERROR)
			fprintf( stderr, "GL CALLBACK: %s source = 0x%x, type = 0x%x, severity = 0x%x, message = %s\n",
										 ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
																 type, source, severity, message );
	}
};

CTEngine::CTEngine(const char* name)
	: mMovementSpeed(1.0f)
	, mLookSpeed(10.0f)
	, mCaptureCursor(true)
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

	mWidth = vidmode->width/1;
	mHeight = vidmode->height/1;
	mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, NULL, NULL);

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

	// During init, enable debug output
	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( OpenGLDebugger::MessageCallback, 0 );

	//get initial position of mouse
	glfwGetCursorPos(mWindow, &mPreviousMouseX, &mPreviousMouseY);


	//Setup camera
	mCamera = new Camera(vidmode->width, vidmode->height, 45.0f);
	mCamera->initialize();

	//Create gui
	mGUI = new GUI();
	if(!mGUI->initialize()) {
		return false;
	}

	//Simple cube shader
	Shader* cubeVertexShader = new Shader("../shaders/cube.vert", GL_VERTEX_SHADER);
	Shader* cubeFragmentShader  = new Shader("../shaders/cube.frag", GL_FRAGMENT_SHADER);
	Program* cubeProgram = new Program("simpleCube");
	cubeProgram->attach(cubeVertexShader);
	cubeProgram->attach(cubeFragmentShader);
	cubeProgram->linkProgram();
	ShaderManager::getInstance()->addShader(cubeProgram);

	//bounding box shader
	Shader* boundingBoxVert = new Shader( "../shaders/boundingbox.vert", GL_VERTEX_SHADER);
	Shader* boundingBoxFrag = new Shader( "../shaders/boundingbox.frag", GL_FRAGMENT_SHADER);
	Program* boundingBoxProgram  = new Program("bbox");
	boundingBoxProgram->attach(boundingBoxVert);
	boundingBoxProgram->attach(boundingBoxFrag);
	boundingBoxProgram->linkProgram();
	ShaderManager::getInstance()->addShader(boundingBoxProgram);

	//Simple cube
	mCube = new Cube(*cubeProgram);
	mCube->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	//initialize Terrain
	//will generate the initial chunks
	mTerrain.initialize(mCamera);

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
	mFPSCounter.calculateFPS(dt);
	sprintf(title,"%s, %.2f FPS %.4f ms", mTitle, mFPSCounter.latestCount, dt);
	glfwSetWindowTitle(mWindow, title);

	if(glfwGetKey(mWindow, GLFW_KEY_ESCAPE)) {
		mRunning = false;
		glfwSetWindowShouldClose(mWindow, GL_TRUE);
	}

	if(glfwGetKey(mWindow, GLFW_KEY_F5)) {
		std::cout << "updating shaders...";
		ShaderManager::getInstance()->updateShaders();
		std::cout << "Done!" << std::endl;
	}

	if(glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT)) {
		if(mCaptureCursor)
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		mCaptureCursor = !mCaptureCursor;
	}

	double mouseX, mouseY;
	glfwGetCursorPos(mWindow, &mouseX, &mouseY);

	double mouseDeltaX = mouseX - mPreviousMouseX;
	double mouseDeltaY = mouseY - mPreviousMouseY;

	mPreviousMouseX = mouseX;
	mPreviousMouseY = mouseY;

	float cameraSpeed = 1.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT))
		cameraSpeed *= 3.0f;
	if(glfwGetKey(mWindow,GLFW_KEY_W))
		mCamera->translate(cameraSpeed * mCamera->mFacing);
	if(glfwGetKey(mWindow,GLFW_KEY_S))
		mCamera->translate(-cameraSpeed * mCamera->mFacing);
	if(glfwGetKey(mWindow,GLFW_KEY_A))
		mCamera->translate(-glm::normalize(glm::cross(mCamera->mFacing, glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);
	if(glfwGetKey(mWindow,GLFW_KEY_D))
		mCamera->translate(glm::normalize(glm::cross(mCamera->mFacing, glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);

	if(mCaptureCursor) {
		float rotatespeed = 0.005f;
		mCamera->mFacing = glm::rotateY(mCamera->mFacing, -(float)mouseDeltaX * rotatespeed);
		mCamera->mFacing.y -= rotatespeed * mouseDeltaY;
		mCamera->mFacing = glm::normalize(mCamera->mFacing);
	}


	mCamera->update(dt);
	mCube->update(dt);

	mTerrain.update();
}

void CTEngine::render(double dt) {
	glViewport(0, 0, mWidth, mHeight);
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	mCube->render(dt);
	mTerrain.render();

	glm::vec3 camPos = mCamera->getPosition();
	glm::vec3 camRot = mCamera->mFacing;
	char camPosText[64];
	char camRotText[64];
	sprintf(camPosText,"x: %.3f, y: %.3f, z: %.3f", camPos.x, camPos.y, camPos.z);
	sprintf(camRotText,"x: %.3f, y: %.3f, z: %.3f", camRot.x, camRot.y, camRot.z);
	mGUI->renderText("Camera:", 10.0f, vidmode->height-20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	mGUI->renderText(camPosText, 10.0f, vidmode->height-40.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	mGUI->renderText(camRotText, 10.0f, vidmode->height-60.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	GLint nTotalMemoryInKB = 0;
	glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
                       &nTotalMemoryInKB );

	GLint nCurAvailMemoryInKB = 0;
	glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
                       &nCurAvailMemoryInKB );
	char memory[64];
	sprintf(memory,"GPU mem: %d MB / %d MB", nCurAvailMemoryInKB / 1000, nTotalMemoryInKB / 1000);
	mGUI->renderText(memory, 10.0f, vidmode->height-80.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown() {
	ShaderManager::getInstance()->disposeShaders();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
