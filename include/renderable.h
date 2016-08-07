/**
* @file renderable.h
* @author Erik Sandrén
* @date 15-12-2015
* @brief renderable entity in CTEngine
*/

#ifndef __RENDERABLE_H__
#define __RENDERABLE_H__

class Renderable : public Entity
{
public:
  virtual void render();
};

#endif
