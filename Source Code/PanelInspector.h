#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include <vector>

class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector();
	~PanelInspector();

	void Draw(ImGuiWindowFlags flags);

	void UpdatePosition(int, int);

private:


};

#endif //__PANEL_INSPECTOR_H__
