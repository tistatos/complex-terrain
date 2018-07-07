/**
* @file chunk.cpp
* @author Erik Sandrén
* @date 27-11-2016
* @brief [Description Goes Here]
*/

#include "chunk.h"

Chunk::Chunk() {
	mEmpty = true;
}

void Chunk::generateBuffers() {
	glGenBuffers(1, &mVertexBuffer);
	glGenBuffers(1, &mIndexBuffer);
	glGenQueries(1, &mIndexQuery);

	//feedback for vertices
	glGenTransformFeedbacks(1, &mVertexFeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mVertexFeedback);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVertexBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	//feedback for indices
	glGenTransformFeedbacks(1, &mIndexFeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mIndexFeedback);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mIndexBuffer);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


	//data to store vertices
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//attributes are position and normals
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(chunkVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(chunkVertex), (GLvoid*)offsetof(chunkVertex, normal));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBindVertexArray(0);
}

void Chunk::fill() {
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(chunkVertex) * MAX_VERTICES, NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * MAX_INDICIES, NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mEmpty = false;
}

void Chunk::setEmpty() {
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mEmpty = true;
}

void Chunk::startVertex() {
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mVertexFeedback);
	glBeginTransformFeedback(GL_POINTS);
}

void Chunk::endVertex() {
	glEndTransformFeedback();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void Chunk::startIndex() {
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mIndexFeedback);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mIndexQuery);
	glBeginTransformFeedback(GL_POINTS);
}

void Chunk::endIndex() {
	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectuiv(mIndexQuery, GL_QUERY_RESULT, &mIndexCount);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void Chunk::render() {
	glBindVertexArray(mVertexArray);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
}
