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
	Terrain();
	void initialize(Camera* camera);
	void render();
	void update();
	void pregenerateChunks();

	const unsigned int getCulledChunks() const { return mCulledChunks; }
	const unsigned int getEmptyChunks() const { return mEmptyChunks; }

private:
	void createShaders();

	void invalidateChunkPositions();

	void generateTextures();

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

	//density generation points
	GLuint mGeoVertexArrayObject;
	GLuint mGeoBuffer;

	//Vertex generation points
	GLuint mGenVertexArrayObject;
	GLuint mGenBuffer;

	//list of all chunks
	Chunk* mChunkList;
	unsigned int mEmptyChunks;
	unsigned int mCulledChunks;

	//list of chunks to be generated
	std::deque<Chunk*> mChunkLoadQueue;

	/*glm::vec3* mChunkPositions;*/
};


#endif
