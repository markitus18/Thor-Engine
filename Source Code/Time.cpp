#include "Time.h"

float Time::scaleTime = 1;
float Time::time;

bool Time::running = true;

Timer Time::gameTimer;

void Time::Start(int maxFPS)
{
	running = true;
}

void Time::PreUpdate(float dt)
{
	deltaTime = running ? dt : 0;
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
	running = false;
	gameTimer.Stop();
}

void Time::Resume()
{
	running = true;
	gameTimer.Resume();
}