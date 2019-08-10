#pragma once

#include <chrono>

class GameTimer
{
public:
	GameTimer();
	float Mark();
	float Peek() const;

	float TotalTime()const;
	float DeltaTime()const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	std::chrono::steady_clock::time_point last;
};