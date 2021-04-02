/**
 * @file mesh.cpp
 * @author Erik Sandrén
 * @date 07-07-2018
 * @brief Simple mesh class that handles indexed vertex meshes
 */
#include <GL/glew.h>

#include "frustum.h"
#include "mesh.h"
#include "vertex.h"

#include <iostream>

Mesh::Mesh() : mVAO(-1), mVBO(-1), mIBO(-1), mInitialized(false), mIndexCount(0) {}

bool Mesh::InitializeMesh(const Vertex vertices[], const uint32_t vertexCount,
													const uint32_t indices[], const uint32_t indexCount) {
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mIBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

	mIndexCount = indexCount;

	glBindVertexArray(0);

	mInitialized = true;
	return true;
}

const void Mesh::renderMesh() const {
	glBindVertexArray(getVertexBuffer());
	glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

const void Mesh::renderLines() const {
	glBindVertexArray(getVertexBuffer());
	glDrawElements(GL_LINES, getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh Mesh::createFrustum(const Frustum* frustum) {
	const glm::vec3* points = frustum->getFrustumPoints();
	Vertex vertices[] = {
			{points[0], glm::vec3(1.0, 1.0, 0.0)}, {points[1], glm::vec3(1.0, 1.0, 0.0)},
			{points[2], glm::vec3(1.0, 1.0, 0.0)}, {points[3], glm::vec3(1.0, 1.0, 0.0)},
			{points[4], glm::vec3(1.0, 1.0, 0.0)}, {points[5], glm::vec3(1.0, 1.0, 0.0)},
			{points[6], glm::vec3(1.0, 1.0, 0.0)}, {points[7], glm::vec3(1.0, 1.0, 0.0)}};

	uint32_t indices[] = {// Near plane
												0, 1, 1, 3, 2, 3, 2, 0,
												////Far plane
												4, 5, 5, 7, 6, 7, 6, 4,
												////Sides
												0, 4, 1, 5, 2, 6, 3, 7};

	Mesh frustumMesh;
	frustumMesh.InitializeMesh(vertices, 8, indices, 24);

	return frustumMesh;
}

Mesh Mesh::createCube(const float cubeSize, const bool outlinesOnly) {
	Vertex vertices[] = {{glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec3(1.0f, 0.0f, 0.0f)},
											 {glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec3(0.0f, 1.0f, 0.0f)},
											 {glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec3(0.0f, 0.0f, 1.0f)},
											 {glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec3(1.0f, 1.0f, 1.0f)},
											 {glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec3(1.0f, 1.0f, 1.0f)},
											 {glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec3(1.0f, 0.0f, 0.0f)},
											 {glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec3(1.0f, 0.0f, 0.0f)},
											 {glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec3(0.0f, 1.0f, 0.0f)}};

	Mesh cube;
	if (outlinesOnly) {
		uint32_t indices[] = {// Front
													0, 1, 1, 2, 2, 3, 3, 0,
													// Back
													4, 5, 5, 7, 6, 7, 6, 4,
													// Sides
													0, 5, 1, 4, 2, 6, 3, 7};
		cube.InitializeMesh(vertices, 8, indices, 24);
	}

	else {
		uint32_t indices[] = {// front
													2, 1, 0, 0, 3, 2,
													// right
													6, 4, 1, 2, 6, 1,
													// back
													7, 5, 4, 6, 7, 4,
													// left
													3, 0, 5, 7, 3, 5,
													// top
													6, 2, 3, 7, 6, 3,
													// bottom
													1, 4, 5, 0, 1, 5};
		cube.InitializeMesh(vertices, 8, indices, 36);
	}

	return cube;
}
