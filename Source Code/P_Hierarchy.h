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

private:
	void DrawRootChilds(TreeNode* node, ImGuiTreeNodeFlags default_flags);
	void DrawGameObject(TreeNode* node, ImGuiTreeNodeFlags default_flags);
	void DisplayGameObjectNode(TreeNode* node, ImGuiTreeNodeFlags defaultFlags);

	//User input management
	void HandleUserInput(TreeNode* node);
	void HandleArrows();
	void DoShiftSelection(TreeNode* selected, bool select);

	void FinishDrag(bool drag);

	bool IsHighlighted(TreeNode* node) const;
	//Returns true if one of the childs in the vector is set to the parent
	bool SetParentByPlace(TreeNode* parent, std::vector<TreeNode*> childs, TreeNode* next = nullptr);

private:
	bool windowFocused = false;

public:
	bool selecting = false;
};

#endif