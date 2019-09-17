#pragma once

#include <chrono>

class GameTimer
{
public:
	GameTimer();
	
	//chrono stuff
	float Mark();
	float Peek() const;

	//non-chrono stuff
	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	/* In essence this entire class is an elaberate stop watch
	that will be used to keep track of frame counts pause time
	etc*/

	/// All the basic mechanics of our watch that will track time
	/// calcutalted in Seconds!

	void Reset(); // Call before message loop
	void Start();
	void Stop();
	void Tick(); // increment every frame
private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;

	std::chrono::steady_clock::time_point last;
};