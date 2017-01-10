#include "timer.hpp"

Timer::Timer () {}
Timer::~Timer () {}

void Timer::Reset () {
	startTime = std::chrono::system_clock::now();
}


std::chrono::duration<float> Timer::GetDuration () {
	TimePoint endTime = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedSeconds = endTime - startTime;
	return elapsedSeconds;
}

float Timer::GetTimeInSeconds () {
	return GetDuration().count();
}

float Timer::GetTimeInMiliseconds () {
	return std::chrono::duration_cast<std::chrono::milliseconds>(GetDuration()).count();
}
