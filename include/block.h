/**
* @file block.h
* @author Erik Sandrén
* @date 15-12-2015
* @brief Block with voxels to describe its density
*/

#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "engine.h"
#include "renderable.h"

class Block : public Renderable
{
public:
  void initializeBlock();
  void clearBlock();
  void render();
  bool isEmpty() { return mIsEmpty; }
private:
  bool mIsEmpty;
  GLuint mVertexArrayID;
  GLuint mVertexBufferID;
  GLuint mVertexFeedbackID;

  GLuint mIndexBufferID;
  GLuint mIndexFeedbackID;
};

#endif

