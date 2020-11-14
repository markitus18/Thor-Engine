#include "Time.h"

float Time::deltaTime;

float Time::scaleTime = 1;
float Time::time;

bool Time::running = false;
bool Time::paused = false;

Timer Time::gameTimer;

void Time::Start(int maxFPS)
{
	running = true;
	gameTimer.Start();
	time = 0;
}

void Time::PreUpdate(float dt)
{
	deltaTime =  dt;
	// TODO: Calculate game time
	if (running)
		time = gameTimer.ReadSec();
}

void Time::Update()
{

}

void Time::Play()
{
	running = true;
}

void Time::Pause()
{
	paused = true;
	gameTimer.Stop();
}

void Time::Resume()
{
	paused = false;
	gameTimer.Resume();
}

void Time::Stop()
{
	running = false;
	gameTimer.Stop();
}