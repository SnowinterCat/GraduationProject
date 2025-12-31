#pragma once
#include <gp/config.hpp>

#include <chrono>

struct Timer {
	Timer();
	float AllTime();
	float Delta();
	float Peek();
private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point last;
};
