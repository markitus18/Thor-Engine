#ifndef __SELECTABLE_H__
#define __SELECTABLE_H__

#include <vector>
#include <string>
#include "Globals.h"

enum TreeNode_Type
{
	GAMEOBJECT,
	RESOURCE,
};

class TreeNode
{
public:
	TreeNode(TreeNode_Type type) : type(type) {}

	TreeNode_Type GetType() const { return type; }

	virtual std::vector<TreeNode*> GetChilds() const { std::vector<TreeNode*> ret; return ret; };
	virtual const char* GetName() const { return nullptr;};
	virtual unsigned long long GetID() const { return 0; };
	virtual bool IsNodeActive() const { return true; };
	virtual TreeNode* GetParentNode() const { return nullptr;};

	uint GetChildNodeIndex(const TreeNode* child) const;
	TreeNode* GetChildNode(uint index) const;
	TreeNode* GetNextOpenNode() const;
	TreeNode* GetPreviousOpenNode() const;

	//Selection methods
	void Select();
	void Unselect();
	bool IsSelected() const;
	bool IsParentSelected() const;

public:
	bool hierarchyOpen = false;
	bool beenSelected = false;
	bool selected = false;
private:
	TreeNode_Type type;
};

#endif // !__SELECTABLE_H__
