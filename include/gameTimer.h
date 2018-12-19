/**
* @file gameTimer.h
* @author Erik Sandrén
* @date 15-12-2015
* @brief Game timer
*/
#ifndef __GAMETIMER_H__
#define __GAMETIMER_H__

class GameTimer {
public:
	GameTimer();
	void tick();
	const float getDeltaTime() const;
	void startTimer();
	void stopTimer();

private:
	double mPrevious;
	double mCurrent;
	double mDeltaTime;
	double mFPS;
	double mFPSCount;
	int mFrames;
};

#endif
