#ifndef __W_ENGINECONFIG_H__
#define __W_ENGINECONFIG_H__

#include "Window.h"

#include "PerfTimer.h"
#include <vector>
#include <string>

struct ImGuiWindowClass;

class W_EngineConfig : public Window
{
public:
	W_EngineConfig(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID);
	~W_EngineConfig() {}

	void Draw() override;

	void UpdateFPSData(int fps, int ms);
	static inline const char* GetName() { return "Engine Config"; };

private:
	float FPS_data[100];
	float ms_data[100];
};

#endif
