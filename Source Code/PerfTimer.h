#ifndef __j1PERFTIMER_H__
#define __j1PERFTIMER_H__

#include "Globals.h"

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	started_at;
	static uint64 frequency;
};

#endif //__j1PERFTIMER_H__