/**
* @file gameTimer.cpp
* @author Erik Sandrén
* @date 15-12-2015
* @brief game timer
*/
#include "gameTimer.h"

#include <GLFW/glfw3.h>

GameTimer::GameTimer()
{
	mFrames = 0;
	mCurrent = 0.0;
	mPrevious = 0.0;
	mDeltaTime = 0.0;
	mFPSCount = 0.0;
	mFPS = 0.0;

}

void GameTimer::tick()
{
	mCurrent = glfwGetTime();
	mDeltaTime = (mCurrent-mPrevious);
	mPrevious = mCurrent;

	mFPSCount += mDeltaTime;
	if(mFPSCount > 1.0)
	{
		mFPS = (double)mFrames/mFPSCount;
		mFPSCount = 0.0;
		mFrames = 0;
	}

	mFrames++;
}

const float GameTimer::getDeltaTime()
{
	return mDeltaTime*1000;
}
const float GameTimer::getFPS()
{
	return mFPS;
}
