/**
* @file gameTimer.h
* @author Erik Sandrén
* @date 15-12-2015
* @brief Game timer
*/
#ifndef __GAMETIMER_H__
#define __GAMETIMER_H__

class GameTimer
{
public:
	GameTimer();
	void tick();
	const float getDeltaTime();
	const float getFPS();
private:
	double mPrevious;
	double mCurrent;
	double mDeltaTime;
	double mFPS;
	double mFPSCount;
	int mFrames;
};

#endif
