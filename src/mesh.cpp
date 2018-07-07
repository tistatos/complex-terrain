/**
* @file mesh.cpp
* @author Erik Sandrén
* @date 07-07-2018
* @brief Simple mesh class that handles indexed vertex meshes
*/
#include <GL/glew.h>

#include "mesh.h"
#include "vertex.h"

Mesh::Mesh() :
	mVAO(-1), mVBO(-1), mIBO(-1),
	mInitialized(false), mIndexCount(0) { }

bool Mesh::InitializeMesh(const Vertex vertices[], const uint32_t vertexCount,
		const uint32_t indices[], const uint32_t indexCount) {
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mIBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

	mIndexCount = indexCount;

	glBindVertexArray(0);

	mInitialized = true;
	return true;
}

const void Mesh::renderMesh() const {
	glBindVertexArray(getVertexBuffer());
	glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

Mesh Mesh::createCube() {
	Vertex vertices[] = {
		{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
		{ glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
		{ glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
		{ glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
		{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
		{ glm::vec3(-1.0f, 1.0f, 1.0), glm::vec3(0.0f, 1.0f, 0.0f) }
	};

	uint32_t indices[] = {
		//front
		2, 1, 0,
		0, 3, 2,
		//right
		6, 4, 1,
		2, 6, 1,
		//back
		7, 5, 4,
		6, 7, 4,
		//left
		3, 0 ,5,
		7, 3, 5,
		//top
		6, 2, 3,
		7, 6, 3,
		//bottom
		1, 4, 5,
		0, 1, 5
	};


	Mesh cube;
	cube.InitializeMesh(vertices, 8, indices, 36);

	return cube;
}
