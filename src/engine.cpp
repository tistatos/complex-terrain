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

void CTEngine::mouseCallbackFunction(GLFWwindow* window, int button, int action, int mods) {
	CTEngine* engine = (CTEngine*)glfwGetWindowUserPointer(window);

	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		int mode = glfwGetInputMode(window, GLFW_CURSOR);
		if(mode == GLFW_CURSOR_DISABLED)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		engine->mCaptureCursor = !engine->mCaptureCursor;
	}
}


/**
 * @brief
 *
 * F5 = reload shaders
 * Q = toggle detached frustum for debugging
 */
void CTEngine::keyCallbackFunction(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CTEngine* engine = (CTEngine*)glfwGetWindowUserPointer(window);

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		engine->stopRunning();
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if(key == GLFW_KEY_F5 && action == GLFW_PRESS) {
			std::cout << "Updating shaders...";
			ShaderManager::getInstance()->updateShaders();
			std::cout << "Done!" << std::endl;
			engine->mTerrain.pregenerateChunks();
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
		engine->mDetachCameraFrustum = !engine->mDetachCameraFrustum;
		engine->mCamera->detachFrustum(engine->mDetachCameraFrustum);
		if(engine->mDetachCameraFrustum) {
			std::cout << "Detaching camera frustum..." << std::endl;
		}
		else {
			std::cout << "reattaching camera frustum..." << std::endl;
		}
	}
}



CTEngine::CTEngine(const char* name)
	: mCaptureCursor(true)
	, mDetachCameraFrustum(false)
	, mMovementSpeed(1.0f)
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

	mWidth = vidmode->width/1;
	mHeight = vidmode->height/1;
	mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, NULL, NULL);
	glfwSetWindowUserPointer(mWindow, this);

	if (!mWindow) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(0);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, 1);
	glfwSetKeyCallback(mWindow, CTEngine::keyCallbackFunction);
	glfwSetMouseButtonCallback(mWindow, CTEngine::mouseCallbackFunction);
	//Initialize glew
	glewExperimental = GL_TRUE;
	glewInit();

	// During init, enable debug output
	//glEnable              ( GL_DEBUG_OUTPUT );
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

	mCamera->setPosition(glm::vec3(0, 0, -240));
	mCamera->setFacing(glm::vec3(0, 0, 1));
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

	double mouseX, mouseY;
	glfwGetCursorPos(mWindow, &mouseX, &mouseY);

	double mouseDeltaX = mouseX - mPreviousMouseX;
	double mouseDeltaY = mouseY - mPreviousMouseY;

	mPreviousMouseX = mouseX;
	mPreviousMouseY = mouseY;

	Camera* cam = this->mCamera;

	float cameraSpeed = 1.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT))
		cameraSpeed *= 3.0f;
	if(glfwGetKey(mWindow, GLFW_KEY_W))
		cam->translate(cameraSpeed * cam->getFacing());
	if(glfwGetKey(mWindow, GLFW_KEY_S))
		cam->translate(-cameraSpeed * cam->getFacing());
	if(glfwGetKey(mWindow, GLFW_KEY_A))
		cam->translate(-glm::normalize(glm::cross(cam->getFacing(), glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);
	if(glfwGetKey(mWindow, GLFW_KEY_D))
		cam->translate(glm::normalize(glm::cross(cam->getFacing(), glm::vec3(0.f,1.0f,0.0f))) * cameraSpeed);

	if(mCaptureCursor) {
		float rotatespeed = 0.005f;
		glm::vec3 facing = glm::rotateY(mCamera->getFacing(), -(float)mouseDeltaX * rotatespeed);
		facing.y -= rotatespeed * mouseDeltaY;
		mCamera->setFacing(glm::normalize(facing));
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
	if(mCamera->frustumDetached()) {
		mCamera->renderFrustum();
	}

	glm::vec3 camPos = mCamera->getPosition();
	glm::vec3 camRot = mCamera->getFacing();
	char camPosText[64];
	char camRotText[64];
	unsigned int culled, empty;

	sprintf(camPosText,"pos(%.3f,%.3f, %.3f)", camPos.x, camPos.y, camPos.z);
	sprintf(camRotText,"dir(%.3f, %.3f, %.3f)", camRot.x, camRot.y, camRot.z);
	std::string cameraString = std::string("Camera: ") + camPosText + " " + camRotText;
	mGUI->renderText(cameraString, 5.0f, vidmode->height-15.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	empty = mTerrain.getEmptyChunks();
	culled = mTerrain.getCulledChunks();

	GLint nTotalMemoryInKB = 0;
	glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
                       &nTotalMemoryInKB );

	GLint nCurAvailMemoryInKB = 0;
	glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
                       &nCurAvailMemoryInKB );
	char memory[64];
	sprintf(memory,"GPU memory: %d MB / %d MB", nCurAvailMemoryInKB / 1000, nTotalMemoryInKB / 1000);
	mGUI->renderText(memory, 5.0f, vidmode->height-35.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	char cullempty[64];
	sprintf(cullempty,"Chunks culled/empty: %d / %d ", culled, empty);
	mGUI->renderText(cullempty, 5.0f, vidmode->height-45.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown() {
	ShaderManager::getInstance()->disposeShaders();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
