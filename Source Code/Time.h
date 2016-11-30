#ifndef __TIME_H__
#define __TIME_H__

#include "Timer.h"

class Time
{
public:

	static void Start(int maxFPS);
	static void PreUpdate(float dt);
	static void Update();

	static void Play();
	static void Pause();
	static void Resume();
	static void Stop();

public:

	//Engine variables -----


	//----------------------

	//In-game variables ----
	static float deltaTime;

	static float scaleTime;
	static float time;

	static Timer gameTimer;
	static bool running;
	//----------------------
};
#endif //__TIME_H__
