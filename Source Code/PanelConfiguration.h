#ifndef __PANEL_CONFIGURATION_H__
#define __PANEL_CONFIGURATION_H__

#include "Panel.h"
#include <string>

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);

	void Init();

	void UpdateFPSData(int fps, int ms);
private:
	float FPS_data[100];
	float ms_data[100];

public:
	float timerRead = 0;
};
#endif // __PANEL_CONFIGURATION_H__
