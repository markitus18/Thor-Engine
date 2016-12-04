#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();
	void Resume();

	float ReadSec() const;
	Uint32 Read();

	bool IsRunning() const;
private:

	bool	running = true;
	Uint32	started_at = 0;
	Uint32	stopped_at = 0;
};

#endif //__TIMER_H__