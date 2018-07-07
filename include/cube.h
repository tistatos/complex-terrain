/**
* @file cube.h
* @author Erik Sandrén
* @date 07-07-2018
* @brief simple cube mesh
*/

#ifndef __CUBE_H__
#define __CUBE_H__
#include "renderable.h"
#include "entity.h"
#include "mesh.h"

class Program;

class Cube : public Renderable, public Entity {
public:
	Cube(Program& shader);
	void update(const double dt) override;
	void render(const double dt) const override;
private:
	Program& mShader;
	Mesh mMesh;
};


#endif
