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

	void Draw();

	void UpdatePosition(int, int);

private:
	bool transform_open = true;
	bool mesh_open = true;
	bool material_open = true;
};

#endif //__PANEL_INSPECTOR_H__
