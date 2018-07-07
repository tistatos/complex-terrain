/**
* @file mesh.h
* @author Erik Sandrén
* @date 07-07-2018
* @brief simple mesh class that handle indexed vertex meshes
*/

#ifndef __MESH_H__
#define __MESH_H__
#include <GL/glew.h>

struct Vertex;

class Mesh {
public:
	Mesh();
	bool InitializeMesh(
			const Vertex vertices[], const uint32_t vertexCount,
			const uint32_t indices[], const uint32_t indexCount);
  const uint32_t& getIndexCount() const { return mIndexCount; }
	const	GLuint& getVertexBuffer() const { return mVAO; }
	const GLuint& getIndexBuffer() const { return mIBO; }

	const void renderMesh() const;

	static Mesh createCube();

private:
	GLuint mVAO;
	GLuint mVBO;
	GLuint mIBO;
	bool mInitialized;
	uint32_t mIndexCount;
};

#endif
