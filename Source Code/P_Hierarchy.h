#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include <vector>

class GameObject;
typedef int ImGuiTreeNodeFlags;

enum class Selection_Type
{
	NONE,
	NORMAL,
	SHIFT,
	CTRL_PLUS,
	CTRL_MINUS
};

enum class ShiftStages
{
	NONE = 0,
	SELECT_START,
	SELECTION_LOOP,
	SELECTING,
};

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
	void HandleShiftSelection(GameObject* gameObject);

	GameObject* GetFirstHierarchyOpen(GameObject* first, GameObject* second) const;
	std::vector<GameObject*>::const_iterator GetFirstHierarchyOpen(const std::vector<GameObject*>& vector) const;
	GameObject* GetNextHierarchyNode(GameObject* gameObject) const;
	GameObject* GetPreviousHierarchyNode(GameObject* gameObject) const;

	bool IsHighlighted(GameObject* gameObject) const;
	void SetParentByPlace(GameObject* parent, std::vector<GameObject*>& childs, GameObject* next = nullptr);
	void RecalcOpenNodes(GameObject* parent);

private:
	bool windowFocused = false;

	//Shift selection management----------------------------
	GameObject* shiftClickedGO = nullptr;
	ShiftStages shiftSelectionStage = ShiftStages::NONE;
	//Shift mode needs to select or unselect?
	bool shiftSelects = true;
	//Activates at the beggining of the draw loop if shift was presset
	bool checkForShiftSelection = false;
	//Is active between selection GameObject ends
	bool shiftSelectionOn = false;
	//------------------------------------------------------

public:
	Selection_Type selectionType = Selection_Type::NONE;
	bool dragging = false;

};

#endif