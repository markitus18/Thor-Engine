#include "Time.h"

float Time::scaleTime = 1;
float Time::time;

bool Time::running = true;

Timer Time::gameTimer;

void Time::Start()
{
	running = true;
}

void Time::PreUpdate(float dt)
{
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