#ifndef __PANEL_CONFIGURATION_H__
#define __PANEL_CONFIGURATION_H__

#include "Panel.h"
#include "PerfTimer.h"

#include <vector>
#include <string>

struct ConfigTimer
{
	ConfigTimer(const char* text)
	{
		this->text = text;
	}

	std::string text;
	std::string tag;
	uint		text_end = 0;
	PerfTimer	timer;
	double		timer_read = 0.0f;
};

class P_Configuration : public Panel
{
public:
	P_Configuration();
	~P_Configuration();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);

	void Init();

	void UpdateFPSData(int fps, int ms);

	uint AddTimer(const char* text, const char* tag);
	void StartTimer(uint index);
	void ReadTimer(uint index);
	void StopTimer(uint index);

private:
	float FPS_data[100];
	float ms_data[100];

	//Timers data ------------------------
	std::vector<ConfigTimer> timers;
	std::vector<std::string> tags;
	uint text_spacing = 30;

	bool show_culling_timers = false;

	SwapButtons timerButtons;
	//------------------------------------

public:
	float timerRead = 0;
};
#endif // __PANEL_CONFIGURATION_H__
