#ifndef __PANEL_CONFIGURATION_H__
#define __PANEL_CONFIGURATION_H__

#include "Panel.h"

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);
};
#endif // __PANEL_CONFIGURATION_H__
