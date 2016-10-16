#ifndef __PANEL_BUTTONS_H__
#define __PANEL_BUTTONS_H__

#include "Panel.h"

class PanelButtons : public Panel
{
public:
	PanelButtons();
	~PanelButtons();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);
};


#endif //__PANEL_BUTTONS_H__
