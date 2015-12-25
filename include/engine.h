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

#include "camera.h"
class CTEngine
{
public:
  CTEngine(char* name);
  ~CTEngine();
  bool initialize();
  void shutdown();
  void update(double dt);
  void render(double dt);
  bool running() { return mRunning; }
private:
  Camera mCamera;
  bool mRunning;
  const GLFWvidmode* vidmode;
  GLFWwindow* mWindow;
  GLFWmonitor* mMonitor;
  char mTitle[64];
};

#endif

