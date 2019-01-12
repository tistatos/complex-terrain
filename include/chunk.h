/**
* @file chunk.h
* @author Erik Sandrén
* @date 29-07-2016
* @brief Complex Terrain Chunk
*/

#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "AABBox.h"
#include "mesh.h"

typedef struct chunkVertex {
	glm::vec4 position;
	glm::vec3 normal;
} chunkVertex_t;

class Chunk {
public:
	//static const uint32_t CHUNK_SIZE = 32;
	static const uint32_t MAX_VERTICES = 33 * 33 * 33;
	//static const uint32_t MAX_INDICIES = 32 * 32 * 32 * 15;

	Chunk();

	void render();

	void renderBoundingBox();
	const AABBox& getBoundingBox() const { return mBoundingBox; }

	const glm::ivec3& getPosition() const { return mPosition; }
	void setPosition(const glm::ivec3& position);

	void fill();
	void setEmpty();

	void startVertex();
	void endVertex();
	//void startIndex();
	//void endIndex();

	bool isEmpty() { return mEmpty; }


private:
	void generateBuffers();
	GLuint mVertexArray;
	GLuint mVertexBuffer;
	uint32_t mVertexCount;

	glm::ivec3 mPosition;

	GLuint mVertexFeedback;
	GLuint mVertexCountQuery;


	//unsigned int mIndexCount;
	//GLuint mIndexQuery;
	//GLuint mIndexBuffer;
	//GLuint mIndexFeedback;

	AABBox mBoundingBox;
	//GLuint mBoundingBoxArray;
	//GLuint mBoundingBoxBuffer;

	Mesh mBoundingBoxMesh;

	bool mEmpty;
};

#endif
