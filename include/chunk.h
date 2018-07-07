/**
* @file chunk.h
* @author Erik Sandrén
* @date 29-07-2016
* @brief Complex Terrain Chunk
*/

#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

typedef struct chunkVertex {
	glm::vec4 position;
	glm::vec3 normal;
} chunkVertex_t;

class Chunk {
public:
	static const uint32_t CHUNK_SIZE = 32;
	static const uint32_t MAX_VERTICES = 33 * 33 * 33;
	static const uint32_t MAX_INDICIES = 32 * 32 * 32 * 15;

	Chunk();

	void render();
	void setEmpty();
	void fill();

	void startVertex();
	void endVertex();
	void startIndex();
	void endIndex();

	bool isEmpty() { return mEmpty; }

	glm::vec3 position;

private:
	void generateBuffers();
	GLuint mVertexArray;
	GLuint mVertexBuffer;
	GLuint mVertexFeedback;

	unsigned int mIndexCount;
	GLuint mIndexQuery;
	GLuint mIndexBuffer;
	GLuint mIndexFeedback;

	bool mEmpty;
};

#endif
