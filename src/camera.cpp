/**
* @file camera.cpp
* @author Erik Sandrén
* @date 20-12-2015
* @brief Camera class
*/

#include "camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::translate(glm::vec3 trans)
{
  mViewMatrix =  glm::translate(mViewMatrix, trans);
}

void Camera::rotate(glm::mat4 rot)
{
  mViewMatrix *= rot;
}

