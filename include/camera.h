/**
* @file camera.h
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera clas
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
public:
  Camera();
  ~Camera();
  void setProjectionMatrix(glm::mat4 proj) {mProjectionMatrix = proj; };

  glm::mat4 getProjectionMatrix() { return mProjectionMatrix; };
  glm::mat4 getViewMatrix() { return mProjectionMatrix; };
  glm::mat4 getViewProjectionMatrix() { return mProjectionMatrix * mViewMatrix; };

  void translate(glm::vec3 trans);
  void rotate(glm::mat4 rot);
private:
  glm::mat4 mViewMatrix;
  glm::mat4 mProjectionMatrix;

};

#endif

