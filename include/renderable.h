/**
* @file renderable.h
* @author Erik Sandrén
* @date 15-12-2015
* @brief renderable entity in CTEngine
*/

#ifndef __RENDERABLE_H__
#define __RENDERABLE_H__

class Renderable {
public:
	virtual void update(const double dt) = 0;
	virtual void render(const double dt) const = 0;
};

#endif
