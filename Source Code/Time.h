#ifndef __TIME_H__
#define __TIME_H__

#include "Timer.h"

class Time
{
public:

	static void Start();
	static void PreUpdate(float dt);
	static void Update();

	static void Play();
	static void Pause();

public:
	static float scaleTime;
	static float time;

	static Timer gameTimer;

private:
	static bool running;
};
#endif //__TIME_H__
