#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include <vector>

class GameObject;
typedef int ImGuiTreeNodeFlags;

class P_Hierarchy : public Panel
{
public:
	P_Hierarchy();
	~P_Hierarchy();

	void Draw(ImGuiWindowFlags flags);
	void DrawRootChilds(GameObject* gameObject, ImGuiTreeNodeFlags default_flags);
	void DrawGameObject(GameObject* gameObject, ImGuiTreeNodeFlags default_flags);

	void UpdatePosition(int, int);

	//GameObject selection methods
	void SelectSingle(GameObject* gameObject, bool openTree = true);
	void AddSelect(GameObject* gameObject, bool openTree = true);
	void UnselectSingle(GameObject* gameObject);
	void UnselectAll();
	
	std::vector<GameObject*> selectedGameObjects;
};

#endif