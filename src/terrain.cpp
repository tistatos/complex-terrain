/**
* @file terrain.cpp
* @author Erik Sandrén
* @date 07-08-2016
* @brief [Description Goes Here]
*/

#include "camera.h"
#include "chunk.h"
#include "marchingCubeLoopkup.h"
#include "shader.h"
#include "shaderManager.h"
#include "terrain.h"
#include "utils.h"

#include <glm/gtx/string_cast.hpp>
static const uint32_t DENSITY_TEXTURE_MARGINS = 2;
static const uint32_t DENSITY_TEXTURE_SIZE = 33;
static const uint32_t DENSITY_TEXTURE_BOUNDS = DENSITY_TEXTURE_SIZE + 2 * DENSITY_TEXTURE_MARGINS;

void Terrain::initialize(Camera* camera) {
	//Set camera and get its intial chunk index position
	mCamera = camera;
	//Determine index of chunk camera is at
	mCameraChunk = getCameraChunk();

	//Create shaders used for generation and rendering
	createShaders();

	//Generate points to to evalute the density function
	glm::ivec3 geoPoints[CHUNK_SIZE+1][CHUNK_SIZE+1][CHUNK_SIZE+1];
	for (unsigned int z = 0; z < CHUNK_SIZE+1; ++z)
		for (unsigned int y = 0; y < CHUNK_SIZE+1; ++y)
			for (unsigned int x = 0; x < CHUNK_SIZE+1; ++x)
				geoPoints[z][y][x] = glm::ivec3(x,y,z);

	//Geo points used during density generation
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
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexFeedbackBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	//glGenVertexArrays(1, &mVertexFeedbackArrayObject);
	//glBindVertexArray(mVertexFeedbackArrayObject);
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexFeedbackBuffer);
	//glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(0);

	generateTextures();

	mChunkList = new Chunk[CHUNK_COUNT];
	/*mChunkPositions = new glm::vec3[CHUNK_COUNT];*/

	pregenerateChunks();
}

glm::ivec3 Terrain::getCameraChunk() const {
	return getChunkIndex(mCamera->getPosition());
	//FIXME: this is the value used for the camera indexing but is wrong for other chunks
	//glm::vec3((CHUNK_SIZE / 2), -(int(CHUNK_SIZE) / 2), (CHUNK_SIZE / 2)))
}

glm::ivec3 Terrain::getChunkIndex(const glm::vec3& position) const {
	glm::ivec3 chunkIndex =
		glm::ivec3( position) / int(CHUNK_SIZE);
	return chunkIndex;
}

uint32_t Terrain::getChunkArrayIndex(const glm::vec3& position) const {
	glm::ivec3 chunkIndex = getChunkIndex(position);
	glm::ivec3 modIdx = glm::ivec3(
					(chunkIndex.x % (int)VERTICAL_CHUNK_COUNT + VERTICAL_CHUNK_COUNT) % VERTICAL_CHUNK_COUNT,
					(chunkIndex.y % (int)HORIZONTAL_CHUNK_COUNT + HORIZONTAL_CHUNK_COUNT) % HORIZONTAL_CHUNK_COUNT,
					(chunkIndex.z % (int)VERTICAL_CHUNK_COUNT + VERTICAL_CHUNK_COUNT) % VERTICAL_CHUNK_COUNT );
	int idx = modIdx.x * VERTICAL_CHUNK_COUNT + modIdx.y * VERTICAL_CHUNK_COUNT * VERTICAL_CHUNK_COUNT + modIdx.z;

	return idx;
}



void Terrain::updateChunkPositions() {
	//TODO: use the camera chunk position together with the
	//vertical horizontal count to calculate an index for the chunks
	glm::ivec3 cameraPosition = getCameraChunk() * int(CHUNK_SIZE);
	glm::ivec3 cameraChunk = getCameraChunk();
	std::cout << "test: " << -1 % 3 << " " << 1 % 3 << " "<<  -1/3 << std::endl;

	for(int x = 0; x < VERTICAL_CHUNK_COUNT; x++) {
		for(int y = 0; y < HORIZONTAL_CHUNK_COUNT; y++) {
			for(int z = 0; z < VERTICAL_CHUNK_COUNT; z++) {
				const glm::ivec3 position = glm::ivec3(
						(x - int(VERTICAL_CHUNK_COUNT - 1) / 2) * 32.0,
						(y - int(HORIZONTAL_CHUNK_COUNT - 1) / 2) * 32.0,
						(z - int(VERTICAL_CHUNK_COUNT - 1) / 2) * 32.0) + cameraPosition;
				const uint32_t flatidx = getChunkArrayIndex(position);
				const glm::ivec3 chunkIndex = getChunkIndex(position);

				const uint32_t idx = flatidx;

				//FIXME: this position test doesnt work. since default value of the chunk is 0,0,0 - the
				//origin chunk isn't loaded
				Chunk* c = &mChunkList[idx];
				if(c->position != position)  {

					std::cout << "Position: " << glm::to_string(position) <<
					" Chunk index: " << glm::to_string(chunkIndex) <<
					" => " << flatidx << std::endl;
					c->position = position;
					mChunkLoadQueue.push(c);
				}
			}
		}
	}
}

void Terrain::pregenerateChunks() {
	updateChunkPositions();
	generateChunks(false);
}

void Terrain::generateChunks(bool limit) {
	const static uint32_t CHUNKS_PER_FRAME = 5;

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, DENSITY_TEXTURE_BOUNDS, DENSITY_TEXTURE_BOUNDS);
	glBindVertexArray(mGeoVertexArrayObject);

	uint32_t chunksLoadedThisFrame = 0;
	while(
			(limit && chunksLoadedThisFrame <= CHUNKS_PER_FRAME && !mChunkLoadQueue.empty()) ||
			(!limit && !mChunkLoadQueue.empty())) {
		Chunk* c = mChunkLoadQueue.front();
		buildDensity(c);
		generateVertices(c);
		if(!c->isEmpty())
			chunksLoadedThisFrame++;
		mChunkLoadQueue.pop();
	}
}

void Terrain::buildDensity(Chunk* c) {
	//get density shader
	Program* p = ShaderManager::getInstance()->getShader("densityProgram");
	p->useProgram();
	//generate density texture for this chunk
	glUniform3i(glGetUniformLocation(*p, "chunkWorldPosition"), c->position.x, c->position.y, c->position.z);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, DENSITY_TEXTURE_BOUNDS);
}

void Terrain::generateVertices(Chunk* c) {

	//Get vertex generator shader
	Program* p = ShaderManager::getInstance()->getShader("vertexProgram");
	p->useProgram();
	//set value of chunk position
	glUniform3f(glGetUniformLocation(*p, "chunkWorldPosition"), c->position.x, c->position.y, c->position.z);

	//skip rasterization
	glEnable(GL_RASTERIZER_DISCARD);
	//FIXME: if we call fill here we create new buffers - running out of GPU memory
	c->fill();
	c->startVertex();
	glBindVertexArray(mGeoVertexArrayObject);
	glDrawArrays(GL_POINTS, 0, (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE +1));
	c->endVertex();
	glDisable(GL_RASTERIZER_DISCARD);
}


void Terrain::generateTextures() {
	//Density map
	glGenTextures(1, &mDensityMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mDensityMap);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F,
			DENSITY_TEXTURE_BOUNDS, DENSITY_TEXTURE_BOUNDS, DENSITY_TEXTURE_BOUNDS);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindImageTexture(1, mDensityMap, 0,  GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	//marching cubes table
	glGenTextures(1, &mTriTable);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mTriTable);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT,
			GL_RED_INTEGER, GL_INT, triTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Terrain::createShaders() {
	//Shader to create density pass
	Shader* instancedQuad = new Shader(
			"../shaders/instancedQuad.vert", GL_VERTEX_SHADER);
	Shader* densityShader = new Shader(
			"../shaders/density.frag", GL_FRAGMENT_SHADER);
	Program* densityProgram = new Program("densityProgram");

	densityProgram->attach(densityShader);
	densityProgram->attach(instancedQuad);
	densityProgram->linkProgram();

	ShaderManager::getInstance()->addShader(densityProgram);

	//shader for Generating of vertices
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
}

// determine if chunk is out side of camera bounds
void Terrain::markOutOfBoundChunks() {
	for (int i = 0; i < CHUNK_COUNT; ++i) {
		Chunk* c = &mChunkList[i];
		const glm::vec3& chunkPosition = c->position;
		const glm::vec3& cameraPosition =mCamera->getPosition();
		if(
			abs(chunkPosition.x - cameraPosition.x) > (VERTICAL_CHUNK_COUNT * CHUNK_SIZE) ||
			abs(chunkPosition.y - cameraPosition.y) > (HORIZONTAL_CHUNK_COUNT * CHUNK_SIZE) ||
			abs(chunkPosition.z - cameraPosition.z) > (VERTICAL_CHUNK_COUNT * CHUNK_SIZE) ) {
			mChunkLoadQueue.push(c);
		}
	}
}

//TODO: add culling to improve fps - intial culling should be on chunks
void Terrain::render() {
	for(unsigned int i = 0; i < CHUNK_COUNT; ++i) {
		Chunk* c = &mChunkList[i];
		Program* render = ShaderManager::getInstance()->getShader("rendering");
		render->useProgram();
		c->render();
		c->renderBoundingBox();
	}
}

void Terrain::update() {
	//determine if camera has moved
	glm::ivec3 currentCameraChunk = getCameraChunk();

	//Camera is in new chunk - update chunk positions
	if(currentCameraChunk != mCameraChunk) {
		std::cout << "transition from chunk: " << std::endl;
		std::cout << glm::to_string(mCameraChunk) << std::endl;
		std::cout << "to:" << std::endl;
		std::cout << glm::to_string(currentCameraChunk) << std::endl;
		markOutOfBoundChunks();
		updateChunkPositions();
	}
	generateChunks(true);
	mCameraChunk = currentCameraChunk;
}
