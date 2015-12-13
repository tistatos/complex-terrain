/**
* @file main.cpp
* @author Erik Sandrén
* @date 12-12-2015
* @brief [Description Goes Here]
*/

#include <iostream>

#include "engine.h"

int main(int argc, char* argv[])
{
  const GLFWvidmode* vidmode;
  GLFWwindow* window;
  GLFWmonitor* monitor;
  char title[64] = "Complex Terrain";

  double dt;
  double fps;

  if(!glfwInit()) return -1;

  monitor = glfwGetPrimaryMonitor();
  vidmode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(vidmode->width/2, vidmode->height/2, title, NULL, NULL);

  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  glewInit();


  while (!glfwWindowShouldClose(window))
  {
    // Set the clear color and depth, and clear the buffers for drawing
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();


    if(glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

