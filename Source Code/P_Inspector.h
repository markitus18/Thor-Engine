#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include <vector>

class GameObject;

class P_Inspector : public Panel
{
public:
	P_Inspector();
	~P_Inspector();

	void Draw(ImGuiWindowFlags flags);

	void UpdatePosition(int, int);
};

#endif //__PANEL_INSPECTOR_H__
