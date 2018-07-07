/**
* @file terrain.cpp
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#include "terrain.h"
#include "chunk.h"
#include "shader.h"
#include "shaderManager.h"
#include "marchingCubeLoopkup.h"

void Terrain::initialize() {

	//Shader to create density pass
	Shader* instancedQuad = new Shader(
			"../shaders/instancedQuad.vert", GL_VERTEX_SHADER);
	Shader* densityShader = new Shader(
			"../shaders/density.frag", GL_FRAGMENT_SHADER);
	Program* densityProgram = new Program("densityProgram");

	//Attach and link shader
	densityProgram->attach(densityShader);
	densityProgram->attach(instancedQuad);
	densityProgram->linkProgram();

	ShaderManager::getInstance()->addShader(densityProgram);

	//shader for Generating of vertcies
	Shader* genVertex = new Shader("../shaders/generateVertex.vert", GL_VERTEX_SHADER);
	Shader* genGeometry = new Shader("../shaders/generateVertex.gs", GL_GEOMETRY_SHADER);
	Program* vertexProgram = new Program("vertexProgram");

	vertexProgram->attach(genVertex);
	vertexProgram->attach(genGeometry);

	//Set varying data - what we want to return from the vertex shader
	const uint32_t varyingCount = 2;
	const char* varyings[varyingCount] = {
		"worldPosition",
		"normal"
	};
	glTransformFeedbackVaryings(*vertexProgram, varyingCount, varyings, GL_INTERLEAVED_ATTRIBS);
	vertexProgram->linkProgram();

	ShaderManager::getInstance()->addShader(vertexProgram);

	//final render shader
	Shader* renderVert = new Shader( "../shaders/render.vert", GL_VERTEX_SHADER);
	Shader* renderFrag = new Shader( "../shaders/render.frag", GL_FRAGMENT_SHADER);
	Program* renderingProgram = new Program("rendering");

	renderingProgram->attach(renderVert);
	renderingProgram->attach(renderFrag);
	renderingProgram->linkProgram();

	ShaderManager::getInstance()->addShader(renderingProgram);

	glm::vec3 geoPoints[CHUNK_SIZE+1][CHUNK_SIZE+1][CHUNK_SIZE+1];
	for (unsigned int x = 0; x < CHUNK_SIZE+1; ++x)
		for (unsigned int y = 0; y < CHUNK_SIZE+1; ++y)
			for (unsigned int z = 0; z < CHUNK_SIZE+1; ++z)
				geoPoints[x][y][z] = glm::vec3(x,y,z);

	//Geo points used during vertex generation
	glGenVertexArrays(1, &mGeoVertexArrayObject);
	glBindVertexArray(mGeoVertexArrayObject);
	glGenBuffers(1, &mGeoBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mGeoBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geoPoints), geoPoints, GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 3, GL_INT, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Vertex transform feedback
	glGenBuffers(1, &mVertexFeedbackBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexFeedbackBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(uint32_t) * (CHUNK_SIZE +1) * (CHUNK_SIZE +1) * (CHUNK_SIZE +1), nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTransformFeedbacks(1, &mVertexFeedbackObject);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mVertexFeedbackObject);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, mVertexFeedbackBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	glGenVertexArrays(1, &mVertexFeedbackArrayObject);
	glBindVertexArray(mVertexFeedbackArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexFeedbackBuffer);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	generateTextures();

	mChunkList = new Chunk[CHUNK_COUNT];
	mChunkPositions = new glm::vec3[CHUNK_COUNT];

	pregenerateChunks();
}

void Terrain::pregenerateChunks() {
	Program* p = ShaderManager::getInstance()->getShader("densityProgram");
	p->useProgram();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, CHUNK_WITH_MARGINS, CHUNK_WITH_MARGINS);
	for(unsigned int i = 0; i < CHUNK_COUNT; ++i) {
		//generate density for the chunk
		Chunk* c = &mChunkList[i];
		c->position = glm::vec3(0.0, 0.0, i);
		glUniform3i(glGetUniformLocation(*p, "chunkWorldPosition"), c->position.x, c->position.y, c->position.z);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, CHUNK_WITH_MARGINS);
		mChunkPositions[i] = c->position;

		//from density, generate vertices
		p = ShaderManager::getInstance()->getShader("vertexProgram");
		p->useProgram();
		glEnable(GL_RASTERIZER_DISCARD);
		c->startVertex();
		glBindVertexArray(mGeoVertexArrayObject);
		glDrawTransformFeedback(GL_POINTS, mVertexFeedbackObject);
		c->endVertex();
		glFlush();
		glDisable(GL_RASTERIZER_DISCARD);
	}
}


void Terrain::generateTextures() {
	//Density map
	glGenTextures(1, &mDensityMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, mDensityMap);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F,
			CHUNK_WITH_MARGINS, CHUNK_WITH_MARGINS, CHUNK_WITH_MARGINS);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindImageTexture(1, mDensityMap, 0,  GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	glBindTexture(GL_TEXTURE_3D, 0);

	//marching cubes table
	glGenTextures(1, &mTriTable);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mTriTable);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT,
			GL_RED_INTEGER, GL_INT, triTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Terrain::render() {
	for(unsigned int i = 0; i < CHUNK_COUNT; ++i) {
		mChunkList[i].render();
	}
}

void Terrain::update() {
	for(unsigned int i = 0; i < CHUNK_COUNT; ++i) {
		mChunkList[i].startVertex();
		//
		//generate vertices
		//
		mChunkList[i].endVertex();

	}
}
