/**
* @file engine.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief Complex terrain engine
*/

#include "engine.h"

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

