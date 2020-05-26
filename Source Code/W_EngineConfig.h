#ifndef __W_ENGINECONFIG_H__
#define __W_ENGINECONFIG_H__

#include "DWindow.h"

#include "PerfTimer.h"
#include <vector>
#include <string>

class W_EngineConfig : public DWindow
{
public:
	W_EngineConfig(M_Editor* editor);
	~W_EngineConfig() {}

	void Draw() override;

	void UpdateFPSData(int fps, int ms);

private:
	float FPS_data[100];
	float ms_data[100];
};

#endif
