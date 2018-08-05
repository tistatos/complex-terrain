/**
* @file terrain.h
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <glm/glm.hpp>
#include <GL/glew.h>

class Chunk;

class Terrain {
public:
	void initialize();
	void render();
	void update();
	static const uint32_t CHUNK_SIZE = 32;
	//static const uint32_t CHUNK_WITH_MARGINS = CHUNK_SIZE + 2 * 2;
	//static const unsigned int CHUNK_COUNT = 10 * 10 * 10;
	static const unsigned int CHUNK_COUNT = 4 * 4 * 4;

private:
	void generateTextures();
	void pregenerateChunks();

	void createShaders();

	//textures
	GLuint mDensityMap;
	GLuint mTriTable;

	//transform feedback
	GLuint mVertexFeedbackBuffer;
	GLuint mVertexFeedbackObject;
	//GLuint mVertexFeedbackArrayObject;

	//Vertex generation points
	GLuint mGeoVertexArrayObject;
	GLuint mGeoBuffer;

	//chunk list
	Chunk* mChunkList;
	glm::vec3* mChunkPositions;
};


#endif
