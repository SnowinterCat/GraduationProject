#include "Timer.h"

Timer::Timer()
{
	start = last = std::chrono::steady_clock::now();
}

float Timer::AllTime()
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();
}

float Timer::Delta()
{
	std::chrono::steady_clock::time_point old = last;
	last = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = last - old;
	return deltaTime.count();
}

float Timer::Peek()
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
}
