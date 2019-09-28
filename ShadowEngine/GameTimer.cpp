#include <Windows.h>
#include "GameTimer.h"
#include <assert.h>

//using namespace std::chrono;

GameTimer::GameTimer()
	:
	mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime()const
{

	
	/* Again we are using the same method as before with keeping time
	its all just a glorified stop watch only we are using windows
	instead of using chrono so its a bit different */

	// first we check to see if we are stopped
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}

	// if not we do our normal time keeping
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
	/// quick note if it is worth the time and effort to re-write everything
	/// using the chrono library I will but for now I know this system so
	/// that is what I will be using
}

float GameTimer::DeltaTime()const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// this is where we keep track of the time on our stop watch
	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	// check to see if we stopped
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	// now we increment the timer
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	// prepare for next frame
	mPrevTime = mCurrTime;

	// and we need to make sure we dont get a negative number due
	// to the system going into power save mode
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}
