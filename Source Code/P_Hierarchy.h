#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include "TreeDisplay.h"
#include <vector>

class GameObject;
typedef int ImGuiTreeNodeFlags;
class TreeNode;

class P_Hierarchy : public Panel, public TreeDisplay
{
public:
	P_Hierarchy();
	~P_Hierarchy();

	void Draw(ImGuiWindowFlags flags);
	void UpdatePosition(int, int);

public:
	bool selecting = false;
};

#endif