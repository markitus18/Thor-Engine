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
	void UpdatePosition(int, int);

private:
	void DrawRootChilds(GameObject* gameObject, ImGuiTreeNodeFlags default_flags);
	void DrawGameObject(GameObject* gameObject, ImGuiTreeNodeFlags default_flags);
	void DisplayGameObjectNode(GameObject* gameObject, ImGuiTreeNodeFlags defaultFlags);

	//User input management
	void HandleUserInput(GameObject* gameObject);
	void HandleArrows();
	void DoShiftSelection(GameObject* selected, bool select);

	GameObject* GetFirstHierarchyOpen(GameObject* first, GameObject* second) const;
	std::vector<GameObject*>::const_iterator GetFirstHierarchyOpen(const std::vector<GameObject*>& vector) const;
	GameObject* GetNextHierarchyNode(GameObject* gameObject) const;
	GameObject* GetPreviousHierarchyNode(GameObject* gameObject) const;
	void FinishDrag(bool drag);

	bool IsHighlighted(GameObject* gameObject) const;
	void SetParentByPlace(GameObject* parent, std::vector<GameObject*> childs, GameObject* next = nullptr);
	void RecalcOpenNodes(GameObject* parent);

private:
	bool windowFocused = false;

public:
	bool selecting = false;
};

#endif