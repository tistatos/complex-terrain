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

	void toggleRenderingBoundingBox() { mRenderBoundingBox = !mRenderBoundingBox; }

	const unsigned int getCulledChunks() const { return mCulledChunks; }
	const unsigned int getEmptyChunks() const { return mEmptyChunks; }

private:
	void createShaders();

	void invalidateChunkPositions();

	void generateTextures();

	void markOutOfBoundChunks();
	void updateChunkPositions();

	void generateChunks(bool limit);
	void buildDensity(Chunk* c);


	// method 1
	void generateVertices(Chunk* c);

	// method 2
	bool listTriangles();
	void generateVertices2(Chunk* c);

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

	//transform feedback for triangles
	GLuint mTriangleListFeedbackObject;
	GLuint mTriangleListFeedbackBuffer;
	GLuint mTriangleListArrayObject;

	//transform feedback for vertices
	//GLuint mVertexFeedbackBuffer;
	//GLuint mVertexFeedbackObject;
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

	//bool settings
	bool mRenderBoundingBox;

	/*glm::vec3* mChunkPositions;*/
};


#endif
