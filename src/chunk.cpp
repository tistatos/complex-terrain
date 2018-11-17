/**
* @file chunk.cpp
* @author Erik Sandrén
* @date 27-11-2016
* @brief Complex Terrain Chunk
*/

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "chunk.h"
#include "shader.h"
#include "shaderManager.h"

Chunk::Chunk() : mPosition (UINT32_MAX), mBoundingBox(glm::vec3(), glm::vec3()) {
	mEmpty = true;
	generateBuffers();
	//mIndexCount = 0;
	mVertexCount  = 0;
}

void Chunk::generateBuffers() {
	glGenBuffers(1, &mVertexBuffer);
	glGenQueries(1, &mVertexCountQuery);
	//glGenBuffers(1, &mIndexBuffer);
	//glGenQueries(1, &mIndexQuery);

	//feedback for vertices
	glGenTransformFeedbacks(1, &mVertexFeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mVertexFeedback);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	//feedback for indices
	//glGenTransformFeedbacks(1, &mIndexFeedback);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mIndexFeedback);
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mIndexBuffer);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


	//data to store vertices
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//attributes are position and normals
	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(chunkVertex), 0);
	//Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(chunkVertex), (GLvoid*)offsetof(chunkVertex, normal));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBindVertexArray(0);

	mBoundingBoxMesh = Mesh::createCube(16.0f, true);
}

void Chunk::setPosition(const glm::ivec3& position) {
	mPosition = position;
	mBoundingBox.setMax(glm::vec3(16.0f) + glm::vec3(position));
	mBoundingBox.setMin(glm::vec3(-16.0f) + glm::vec3(position));
}

void Chunk::fill() {
	if(mEmpty) {
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(chunkVertex) * MAX_VERTICES, NULL, GL_STATIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * MAX_INDICIES, NULL, GL_STATIC_COPY);
		///glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	mEmpty = false;
}

//void Chunk::setEmpty() {
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_COPY);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_COPY);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//mEmpty = true;
//}

void Chunk::startVertex() {
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mVertexFeedback);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mVertexCountQuery);
	glBeginTransformFeedback(GL_POINTS);
}


void Chunk::endVertex() {
	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectuiv(mVertexCountQuery, GL_QUERY_RESULT, &mVertexCount);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	mEmpty = mVertexCount > 0 ? false : true;
}

//void Chunk::startIndex() {
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mIndexFeedback);
	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mIndexQuery);
	//glBeginTransformFeedback(GL_POINTS);
//}

//void Chunk::endIndex() {
	//glEndTransformFeedback();
	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	//glGetQueryObjectuiv(mIndexQuery, GL_QUERY_RESULT, &mIndexCount);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
//}

void Chunk::render() {
	glBindVertexArray(mVertexArray);
	glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
}

void Chunk::renderBoundingBox() {
	Program* bb = ShaderManager::getInstance()->getShader("bbox");
	bb->useProgram();
	glm::mat4 m;
	m = glm::translate(m, glm::vec3(mPosition));

	glUniformMatrix4fv(glGetUniformLocation(*bb, "m"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform1i(glGetUniformLocation(*bb, "empty"), (mVertexCount > 0 ? 0 : 1));
	mBoundingBoxMesh.renderLines();
}
