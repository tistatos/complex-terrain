/**
* @file terrain.cpp
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#include "terrain.h"

Terrain::Terrain() : mDensityProgram("densityProgram") {
}

void Terrain::initialize() {
	Shader* instancedQuad = new Shader(
			"../shaders/instancedQuad.vert", GL_VERTEX_SHADER);
	Shader* densityShader = new Shader(
			"../shaders/density.frag", GL_FRAGMENT_SHADER);

	mDensityProgram.attach(densityShader);
	mDensityProgram.attach(instancedQuad);
	mDensityProgram.linkProgram();
}

void Terrain::render() {}
void Terrain::update() {}
