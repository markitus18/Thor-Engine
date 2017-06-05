#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include <vector>

class GameObject;
typedef int ImGuiTreeNodeFlags;

enum ShiftSelectionStages
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
	void HandleShiftSelection(GameObject* gameObject);

	GameObject* GetNextHierarchyNode(GameObject* gameObject) const;
	GameObject* GetPreviousHierarchyNode(GameObject* gameObject) const;

private:
	bool windowFocused = false;
	GameObject* lastSelected = nullptr;


	//Shift selection management----------------------------
	GameObject* shiftClickedGO = nullptr;
	ShiftSelectionStages shiftSelectionStage = NONE;
	bool shiftSelects = true;
	//------------------------------------------------------

	//Activates at the beggining of the draw loop if shift was presset
	bool checkForShiftSelection = false;
	//Is active between selection GameObject ends
	bool shiftSelectionOn = false;
	//Shift mode needs to select or unselect?

};

#endif