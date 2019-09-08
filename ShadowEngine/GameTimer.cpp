#include "GameTimer.h"

using namespace std::chrono;

GameTimer::GameTimer()
{
	last = steady_clock::now();
}

float GameTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float GameTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}

void GameTimer::Reset()
{
}
//
//float GameTimer::Pause()
//{
//	/*When we call pause we need to stop incrementing the watch
//	to do this we need to keep track of the time passed while 
//	we are paused*/
//
//	duration<float> pauseTime;
//
//	//First we need to know if the application is paused
//	if (isPaused = true)
//	{
//		auto pauseStart = last;
//		steady_clock::time_point pauseEnd;
//		while (isPaused = true)
//		{
//			//while we are paused we need to keep track of time elapsed
//			pauseEnd = steady_clock::now();
//		}
//
//		// Once we arent paused we need to subtract that time from the timer
//		pauseTime = pauseEnd - pauseStart;
//	}
//
//	return pauseTime.count;
//}


void GameTimer::Tick()
{
}
