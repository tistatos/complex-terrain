/**
 * @file terrain.cpp
 * @author Erik Sandrén
 * @date 07-08-2016
 * @brief Complex terrain
 */

#include "terrain.h"
#include "camera.h"
#include "chunk.h"
#include "frustum.h"
#include "gameTimer.h"
#include "marchingCubeLoopkup.h"
#include "shader.h"
#include "shaderManager.h"
#include "utils.h"

#include <algorithm>
#include <glm/gtx/string_cast.hpp>

static const uint32_t CHUNK_SIZE = 32;
static const unsigned int XZ_CHUNK_COUNT = 25;
static const unsigned int Y_CHUNK_COUNT = 9;
static const unsigned int CHUNK_COUNT = XZ_CHUNK_COUNT * XZ_CHUNK_COUNT * Y_CHUNK_COUNT;
static const uint32_t DENSITY_TEXTURE_MARGINS = 8;
static const uint32_t DENSITY_TEXTURE_SIZE = 33;
static const uint32_t DENSITY_TEXTURE_BOUNDS = DENSITY_TEXTURE_SIZE + 2 * DENSITY_TEXTURE_MARGINS;

static const bool GENERATE_NEW_CHUNKS = true;

Terrain::Terrain()
		: mCamera(nullptr), mDensityMap(0), mTriTable(0), mTriangleListFeedbackObject(0),
			mTriangleListFeedbackBuffer(0), mGeoVertexArrayObject(0), mGeoBuffer(0),
			mGenVertexArrayObject(0), mGenBuffer(0), mEmptyChunks(0), mRenderBoundingBox(false),
			mCulledChunks(0) {}

void Terrain::initialize(Camera* camera) {

	// Set camera and get its intial chunk index position
	mCamera = camera;
	// Determine index of chunk camera is at
	mCameraChunk = getCameraChunk();

	// Create shaders used for generation and rendering
	createShaders();

	// Generate points to to evalute the density function
	glm::ivec3 geoPoints[CHUNK_SIZE + 1][CHUNK_SIZE + 1][CHUNK_SIZE + 1];
	for (unsigned int z = 0; z < CHUNK_SIZE + 1; ++z)
		for (unsigned int y = 0; y < CHUNK_SIZE + 1; ++y)
			for (unsigned int x = 0; x < CHUNK_SIZE + 1; ++x)
				geoPoints[z][y][x] = glm::ivec3(x, y, z);

	// FIXME: we should try to avoid using this in the future
	// Points for sampling during vertex generation since this is merely a exact copy of the one above
	// and we should have to use it
	glm::ivec3 genPoints[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
		for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
			for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
				genPoints[z][y][x] = glm::ivec3(x, y, z);

	// Geo points used during density generation
	glGenVertexArrays(1, &mGeoVertexArrayObject);
	glBindVertexArray(mGeoVertexArrayObject);
	glGenBuffers(1, &mGeoBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mGeoBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geoPoints), geoPoints, GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 3, GL_INT, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// FIXME: remove this in the future
	// Geo points used during density generation
	glGenVertexArrays(1, &mGenVertexArrayObject);
	glBindVertexArray(mGenVertexArrayObject);
	glGenBuffers(1, &mGenBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mGenBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(genPoints), genPoints, GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 3, GL_INT, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Triangle list transfrom feedback
	glGenBuffers(1, &mTriangleListFeedbackBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mTriangleListFeedbackBuffer);
	glBufferData(GL_ARRAY_BUFFER,
							 sizeof(uint32_t) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1), nullptr,
							 GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTransformFeedbacks(1, &mTriangleListFeedbackObject);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTriangleListFeedbackObject);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mTriangleListFeedbackBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	glGenVertexArrays(1, &mTriangleListArrayObject);
	glBindVertexArray(mTriangleListArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, mTriangleListFeedbackBuffer);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	generateTextures();

	mChunkList = new Chunk[CHUNK_COUNT];

	// Initialize position to MAX to indicate that it has no position as of yet
	/*mChunkPositions = new glm::vec3[CHUNK_COUNT];*/

	pregenerateChunks();
}

void Terrain::invalidateChunkPositions() {
	const uint32_t startPos = UINT32_MAX;
	for (int i = 0; i < CHUNK_COUNT; ++i) {
		mChunkList[i].setPosition(glm::ivec3(startPos, startPos, startPos));
	}
}
glm::ivec3 Terrain::getCameraChunk() const {
	glm::vec3 cameraPosition = mCamera->getPosition();
	return getChunkIndex(cameraPosition -
											 glm::vec3(int(CHUNK_SIZE) / 2, int(CHUNK_SIZE) / 2, int(CHUNK_SIZE) / 2));
}

glm::ivec3 Terrain::getChunkIndex(const glm::vec3& position) const {
	glm::ivec3 chunkIndex = glm::ivec3(position) / int(CHUNK_SIZE);
	// glm::vec3((CHUNK_SIZE / 2), -(int(CHUNK_SIZE) / 2), (CHUNK_SIZE / 2)))
	return chunkIndex;
}

uint32_t Terrain::getChunkArrayIndex(const glm::vec3& position) const {
	glm::ivec3 chunkIndex = getChunkIndex(position);
	glm::ivec3 modIdx =
			glm::ivec3((chunkIndex.x % (int)XZ_CHUNK_COUNT + XZ_CHUNK_COUNT) % XZ_CHUNK_COUNT,
								 (chunkIndex.y % (int)Y_CHUNK_COUNT + Y_CHUNK_COUNT) % Y_CHUNK_COUNT,
								 (chunkIndex.z % (int)XZ_CHUNK_COUNT + XZ_CHUNK_COUNT) % XZ_CHUNK_COUNT);
	int idx = modIdx.x * XZ_CHUNK_COUNT + modIdx.y * XZ_CHUNK_COUNT * XZ_CHUNK_COUNT + modIdx.z;

	return idx;
}

bool Terrain::listTriangles() {
	// Get vertex generator shader
	Program* p = ShaderManager::getInstance()->getShader("listTriangles");
	p->useProgram();

	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTriangleListFeedbackObject);
	GLuint triangleCountQuery = 0;
	glGenQueries(1, &triangleCountQuery);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, triangleCountQuery);
	glBeginTransformFeedback(GL_POINTS);

	glBindVertexArray(mGenVertexArrayObject);
	const int CHUNK_OFFSET = 0;
	glDrawArrays(GL_POINTS, 0,
							 (CHUNK_SIZE + CHUNK_OFFSET) * (CHUNK_SIZE + CHUNK_OFFSET) *
									 (CHUNK_SIZE + CHUNK_OFFSET));

	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	GLuint triangleCount = 0;
	glGetQueryObjectuiv(triangleCountQuery, GL_QUERY_RESULT, &triangleCount);
	// std::cout << triangleCount << " triangle count" << std::endl;
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDisable(GL_RASTERIZER_DISCARD);

	return triangleCount != 0;
}

void Terrain::generateVertices2(Chunk* c) {
	Program* p = ShaderManager::getInstance()->getShader("genVertices");
	p->useProgram();

	// set value of chunk position
	const glm::ivec3 pos = c->getPosition();
	glUniform3f(glGetUniformLocation(*p, "chunkWorldPosition"), pos.x, pos.y, pos.z);

	// skip rasterization
	glEnable(GL_RASTERIZER_DISCARD);
	c->fill();
	c->startVertex();

	glBindVertexArray(mTriangleListArrayObject);
	glDrawTransformFeedback(GL_POINTS, mTriangleListFeedbackObject);

	c->endVertex();
	glBindVertexArray(0);
	glDisable(GL_RASTERIZER_DISCARD);
}

void Terrain::updateChunkPositions() {
	glm::ivec3 cameraChunk = getCameraChunk();
	glm::ivec3 cameraPosition = cameraChunk * int(CHUNK_SIZE);

	// TODO: start from the camera chunk and work outwards. Currently loading occurs from one way so
	// when switching between planes the loading is quite terrible
	for (int x = 0; x < XZ_CHUNK_COUNT; x++) {
		for (int y = 0; y < Y_CHUNK_COUNT; y++) {
			for (int z = 0; z < XZ_CHUNK_COUNT; z++) {

				const glm::ivec3 position = glm::ivec3((x - int(XZ_CHUNK_COUNT - 1) / 2) * 32.0,
																							 (y - int(Y_CHUNK_COUNT - 1) / 2) * 32.0,
																							 (z - int(XZ_CHUNK_COUNT - 1) / 2) * 32.0) +
																		cameraPosition;
				const uint32_t flatidx = getChunkArrayIndex(position);
				const uint32_t idx = flatidx;

				Chunk* c = &mChunkList[idx];
				if (c->getPosition() != position) {
					c->setPosition(position);
					if(!mCamera->getFrustum().intersects(c->getBoundingBox())) {
						mChunkLoadQueue.push_back(c);
					}
					else {
						//FIXME: this prio doesn't work as expected, the chunk in front DO get prio but the one
						//that is the farthest away gets it.
						mChunkLoadQueue.push_front(c);
					}
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

	uint32_t chunksLoadedThisFrame = 0;
	while ((limit && chunksLoadedThisFrame <= CHUNKS_PER_FRAME && !mChunkLoadQueue.empty()) ||
				 (!limit && !mChunkLoadQueue.empty())) {
		Chunk* c = mChunkLoadQueue.front();

		buildDensity(c);
		// Simple method
		// generateVertices(c);
		// chunksLoadedThisFrame++;
		// mChunkLoadQueue.pop_front();

		// New Chunk generation method
		const bool chunkNotEmpty = listTriangles();
		if (chunkNotEmpty) {
			generateVertices2(c);
			chunksLoadedThisFrame++;
		} else {
			c->setEmpty();
		}
		mChunkLoadQueue.pop_front();
	}

	// if(chunksLoadedThisFrame >= CHUNKS_PER_FRAME)
	// std::cout << "Max chunk per frame met" << std::endl;
}

void Terrain::buildDensity(Chunk* c) {
	// get density shader
	Program* p = ShaderManager::getInstance()->getShader("densityProgram");
	p->useProgram();
	// generate density texture for this chunk
	const glm::ivec3 pos = c->getPosition();
	glBindVertexArray(mGeoVertexArrayObject);
	glUniform3i(glGetUniformLocation(*p, "chunkWorldPosition"), pos.x, pos.y, pos.z);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, DENSITY_TEXTURE_BOUNDS);
	glBindVertexArray(0);
}

void Terrain::generateVertices(Chunk* c) {
	// Get vertex generator shader
	Program* p = ShaderManager::getInstance()->getShader("vertexProgram");
	p->useProgram();

	// set value of chunk position
	const glm::ivec3 pos = c->getPosition();
	glUniform3f(glGetUniformLocation(*p, "chunkWorldPosition"), pos.x, pos.y, pos.z);

	// skip rasterization
	glEnable(GL_RASTERIZER_DISCARD);
	c->fill();
	c->startVertex();
	glBindVertexArray(mGenVertexArrayObject);
	const int CHUNK_OFFSET = 0;
	glDrawArrays(GL_POINTS, 0,
							 (CHUNK_SIZE + CHUNK_OFFSET) * (CHUNK_SIZE + CHUNK_OFFSET) *
									 (CHUNK_SIZE + CHUNK_OFFSET));
	c->endVertex();
	glBindVertexArray(0);
	glDisable(GL_RASTERIZER_DISCARD);
}

void Terrain::generateTextures() {
	// Density map
	glGenTextures(1, &mDensityMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mDensityMap);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, DENSITY_TEXTURE_BOUNDS, DENSITY_TEXTURE_BOUNDS,
								 DENSITY_TEXTURE_BOUNDS);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindImageTexture(1, mDensityMap, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	// marching cubes table
	glGenTextures(1, &mTriTable);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mTriTable);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TRI_TABLE_WIDTH, TRI_TABLE_HEIGHT, GL_RED_INTEGER, GL_INT,
									triTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Terrain::createShaders() {
	// Shader to create density pass
	// Runs pixel shader with a 3D texture and uses simplex noise to generate data for chunks
	Shader* instancedQuad = new Shader("../shaders/instancedQuad.vert", GL_VERTEX_SHADER);
	Shader* densityShader = new Shader("../shaders/density.frag", GL_FRAGMENT_SHADER);
	Program* densityProgram = new Program("densityProgram");

	densityProgram->attach(densityShader);
	densityProgram->attach(instancedQuad);
	densityProgram->linkProgram();

	ShaderManager::getInstance()->addShader(densityProgram);

	// Shader for Generating of vertices
	// Simplest and slowest approach. It samples the density pass to determine marching cube case and
	// generates all data required for the chunk (triangles and ambient occlusion)
	Shader* genVertex = new Shader("../shaders/generateVertex.vert", GL_VERTEX_SHADER);
	Shader* genGeometry = new Shader("../shaders/generateVertex.gs", GL_GEOMETRY_SHADER);
	Program* vertexProgram = new Program("vertexProgram");

	vertexProgram->attach(genVertex);
	vertexProgram->attach(genGeometry);

	// Set varying data - what we want to return from the geometry shader
	const uint32_t genVertexVaryingCount = 2;
	const char* genVertexVaryings[genVertexVaryingCount] = {"worldPosition", "normal"};

	glTransformFeedbackVaryings(*vertexProgram, genVertexVaryingCount, genVertexVaryings,
															GL_INTERLEAVED_ATTRIBS);
	vertexProgram->linkProgram();

	ShaderManager::getInstance()->addShader(vertexProgram);

	// Shader for listing the triangle
	// intermediate approach. Does only output a single 32-bit value with all information about the
	// triangle, but does not generate anything. the data is xyz position in chunk and what edges the
	// triangle should join to
	Shader* listTriangleVertex = new Shader("../shaders/listTriangles.vert", GL_VERTEX_SHADER);
	Shader* listTriangleGeometry = new Shader("../shaders/listTriangles.gs", GL_GEOMETRY_SHADER);
	Program* listTriangleProgram = new Program("listTriangles");

	listTriangleProgram->attach(listTriangleVertex);
	listTriangleProgram->attach(listTriangleGeometry);

	const uint32_t listTrianglesVaryingCount = 1;
	const char* listTrianglesVaryings[listTrianglesVaryingCount] = {
			"xyzEdges",
	};

	glTransformFeedbackVaryings(*listTriangleProgram, listTrianglesVaryingCount,
															listTrianglesVaryings, GL_INTERLEAVED_ATTRIBS);
	listTriangleProgram->linkProgram();

	ShaderManager::getInstance()->addShader(listTriangleProgram);

	// Shader to generate vertices from triangle list
	// intermediate approach, second pass. Will parse the triangle list and generate vertices in the
	// vertex shader instead of geometry shader.
	Shader* genVertFromTriangleVertex =
			new Shader("../shaders/generateVertexFromTriangle.vert", GL_VERTEX_SHADER);
	Shader* genVertFromTriangleGeometry =
			new Shader("../shaders/generateVertexFromTriangle.gs", GL_GEOMETRY_SHADER);
	Program* genVertFromTriangleProgram = new Program("genVertices");
	genVertFromTriangleProgram->attach(genVertFromTriangleVertex);
	genVertFromTriangleProgram->attach(genVertFromTriangleGeometry);

	const uint32_t genVertFromTriangleVaryingCount = 2;
	const char* genVertFromTriangleVaryings[genVertFromTriangleVaryingCount]{"worldPosition",
																																					 "normal"};

	glTransformFeedbackVaryings(*genVertFromTriangleProgram, genVertFromTriangleVaryingCount,
															genVertFromTriangleVaryings, GL_INTERLEAVED_ATTRIBS);
	genVertFromTriangleProgram->linkProgram();
	ShaderManager::getInstance()->addShader(genVertFromTriangleProgram);

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
		const glm::ivec3& chunkPosition = c->getPosition();
		const glm::vec3& cameraPosition = mCamera->getPosition();
		if (abs(chunkPosition.x - cameraPosition.x) > (XZ_CHUNK_COUNT * CHUNK_SIZE) ||
				abs(chunkPosition.y - cameraPosition.y) > (Y_CHUNK_COUNT * CHUNK_SIZE) ||
				abs(chunkPosition.z - cameraPosition.z) > (XZ_CHUNK_COUNT * CHUNK_SIZE)) {
			mChunkLoadQueue.push_back(c);
			// FIXME: this never happens
		}
	}
}

void Terrain::render() {
	mEmptyChunks = 0;
	mCulledChunks = 0;
	Program* render = ShaderManager::getInstance()->getShader("rendering");
	for (unsigned int i = 0; i < CHUNK_COUNT; ++i) {
		Chunk* c = &mChunkList[i];
		if (!mCamera->frustumDetached() && !mCamera->getFrustum().intersects(c->getBoundingBox())) {
			mCulledChunks++;
			continue;
		}

		if (c->isEmpty()) {
			mEmptyChunks++;
		} else {
			render->useProgram();
			c->render();
			if(mRenderBoundingBox)
				c->renderBoundingBox();
		}
	}
}

void Terrain::update() {
	// determine if camera has moved
	glm::ivec3 currentCameraChunk = getCameraChunk();

	//Camera is in new chunk - update chunk positions
	if(currentCameraChunk != mCameraChunk) {
		//std::cout << "transition from chunk: " << glm::to_string(mCameraChunk) << " to:" << glm::to_string(currentCameraChunk) << std::endl;
		if(NEW_CHUNKS) {
			markOutOfBoundChunks();
			updateChunkPositions();
		}
	}
	generateChunks(true);
	mCameraChunk = currentCameraChunk;
}
