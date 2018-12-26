#ifndef __TREE_DISPLAY_H__
#define __TREE_DISPLAY_H__
#include <vector>

class TreeNode;

class TreeDisplay
{
public:
	TreeDisplay();
	~TreeDisplay();

protected:
	void DrawTree(TreeNode* root);
	void DrawNode(TreeNode* node);
	void ShowNode(TreeNode* node);
	void DrawNodeChilds(TreeNode* node);

	void HandleUserInput(TreeNode* node);
	void HandleArrows();
	void DoShiftSelection(TreeNode* selected, bool select);

	bool IsHighlighted(TreeNode* node) const;
	bool SetParentByPlace(TreeNode* parent, std::vector<TreeNode*> childs, TreeNode* next = nullptr);

protected:
	bool selecting = false;
};

#endif