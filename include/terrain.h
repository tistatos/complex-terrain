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
#include <queue>

class Chunk;
class Camera;

class Terrain {
public:
	void initialize(Camera* camera);
	void render();
	void update();
	static const uint32_t CHUNK_SIZE = 32;
	//static const uint32_t CHUNK_WITH_MARGINS = CHUNK_SIZE + 2 * 2;
	static const unsigned int VERTICAL_CHUNK_COUNT = 17;
	static const unsigned int HORIZONTAL_CHUNK_COUNT = 3;
	static const unsigned int CHUNK_COUNT = VERTICAL_CHUNK_COUNT * VERTICAL_CHUNK_COUNT * HORIZONTAL_CHUNK_COUNT;

private:
	void createShaders();

	void generateTextures();
	void pregenerateChunks();

	void markOutOfBoundChunks();
	void updateChunkPositions();

	void buildDensity(Chunk* c);
	void generateVertices(Chunk* c);

	void generateChunks(bool limit);

	glm::ivec3 getCameraChunk() const;
	glm::ivec3 getChunkIndex(const glm::vec3& position) const;
	uint32_t getChunkArrayIndex(const glm::vec3& position) const;
	//camera
	Camera* mCamera;
	glm::vec3 mLastCameraPosition;
	glm::ivec3 mCameraChunk;

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

	//list of all chunks
	Chunk* mChunkList;

	//list of chunks to be generated
	std::queue<Chunk*> mChunkLoadQueue;

	/*glm::vec3* mChunkPositions;*/
};


#endif
