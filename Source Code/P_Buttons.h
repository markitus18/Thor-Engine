#ifndef __PANEL_BUTTONS_H__
#define __PANEL_BUTTONS_H__

#include "Panel.h"

class P_Buttons : public Panel
{
public:
	P_Buttons();
	~P_Buttons();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);
};


#endif //__PANEL_BUTTONS_H__
