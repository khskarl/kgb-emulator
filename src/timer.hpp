#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

class Timer {
private:
	TimePoint startTime;

	std::chrono::duration<float> GetDuration ();
public:
	Timer ();
	virtual ~Timer ();

	void Reset ();

	float GetTimeInSeconds ();
	float GetTimeInMiliseconds ();
};

#endif
