#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"
#include <vector>
#include "MathGeoLib\src\MathGeoLib.h"
class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector();
	~PanelInspector();

	void Draw(ImGuiWindowFlags flags);

	void UpdatePosition(int, int);

private:
	float3 previous_Pos;


};

#endif //__PANEL_INSPECTOR_H__
