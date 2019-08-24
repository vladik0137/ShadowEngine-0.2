#pragma once

#include <chrono>

class GameTimer
{
public:
	GameTimer();
	float Mark();
	float Peek() const;

	/* In essence this entire class is an elaberate stop watch
	that will be used to keep track of frame counts pause time
	etc*/

	/// All the basic mechanics of our watch that will track time
	/// calcutalted in Seconds!

	void Reset(); // Clear the stop watch
	float Pause(); // pause the watch
	void Tick(); // increment the watch
private:

	bool isPaused = false;

	std::chrono::steady_clock::time_point last;
};