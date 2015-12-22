/**
* @file engine.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief Complex terrain engine
*/

#include "engine.h"
#include "shader.h"
CTEngine::CTEngine()
{
  mRunning = false;
  sprintf(mTitle,"Complex Terrain");
}

CTEngine::~CTEngine()
{
}

bool CTEngine::initialize()
{
  if(!glfwInit())
  {
    return false;
  }

  mMonitor = glfwGetPrimaryMonitor();
  vidmode = glfwGetVideoMode(mMonitor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  mWindow = glfwCreateWindow(vidmode->width/2, vidmode->height/2, mTitle, NULL, NULL);

  if (!mWindow)
  {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(0);

  glewInit();

  float fov = 45.0f;
  mCamera.setProjectionMatrix(glm::perspective(
        fov,
        (float)(vidmode->width/vidmode->height),
        0.1f,
        100.0f)
  );

  Program p;
  Shader vert("../shaders/standard.vert", GL_VERTEX_SHADER);
  Shader frag("../shaders/standard.frag", GL_FRAGMENT_SHADER);
  p.attach(vert);
  p.attach(frag);
  p.linkProgram();
  mRunning = true;
  return true;
}

void CTEngine::update(double dt)
{
  glfwPollEvents();

  sprintf(mTitle,"Complex Terrain, %.4f ms", dt);
  glfwSetWindowTitle(mWindow, mTitle);
  if(glfwGetKey(mWindow, GLFW_KEY_ESCAPE))
  {
    mRunning = false;
    glfwSetWindowShouldClose(mWindow, GL_TRUE);
  }

  glm::vec3 movement;

  if(glfwGetKey(mWindow, GLFW_KEY_W))
    movement.z += 1.0f;
  if(glfwGetKey(mWindow, GLFW_KEY_A))
    movement.z -= 1.0f;
  if(glfwGetKey(mWindow, GLFW_KEY_S))
    movement.x += 1.0f;
  if(glfwGetKey(mWindow, GLFW_KEY_D))
    movement.x -= 1.0f;
}

void CTEngine::render(double dt)
{
  glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glfwSwapBuffers(mWindow);
}

void CTEngine::shutdown()
{
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

