#include "TreeNode.h"

uint TreeNode::GetChildNodeIndex(const TreeNode* child) const
{
	int count = 0;
	std::vector<TreeNode*>::const_iterator it;
	for (it = GetChilds().begin(); it != GetChilds().end(); ++it)
	{
		if ((*it) == child)
		{
			return count;
		}
		++count;
	}
	return -1;
}

TreeNode* TreeNode::GetChildNode(uint index) const
{
	std::vector<TreeNode*> childs = GetChilds();
	if (index < childs.size())
	{
		return childs[index];
	}
	return nullptr;
}

TreeNode* TreeNode::GetNextOpenNode() const
{
	TreeNode* ret = nullptr;
	TreeNode* parent = GetParentNode();

	std::vector<TreeNode*> childs = GetChilds();
	if (childs.size() > 0 && hierarchyOpen == true)
		return *childs.begin();

	const TreeNode* toEvaluate = this;
	while (parent != nullptr)
	{
		if (parent->hierarchyOpen == true)
		{
			int childIndex = parent->GetChildNodeIndex(toEvaluate);
			//Returning next "sibling" if exists and is open
			if (ret = parent->GetChildNode(++childIndex))
				return ret;
		}
		toEvaluate = parent;
		parent = parent->GetParentNode();
	}
	return nullptr;
}

TreeNode* TreeNode::GetPreviousOpenNode() const
{
	TreeNode* ret = nullptr;
	int childIndex = GetParentNode()->GetChildNodeIndex(this);
	if (childIndex > 0)
	{
		TreeNode* previous = GetParentNode()->GetChildNode(--childIndex);
		while (previous->hierarchyOpen == true)
		{
			previous = previous->GetChildNode(previous->GetChilds().size() - 1);
		}
		return previous;
	}
	else
	{
		return GetParentNode();
	}
}

void TreeNode::Select()
{
	selected = true;
}

void TreeNode::Unselect()
{
	selected = false;
}

bool TreeNode::IsSelected() const
{
	return selected;
}

bool TreeNode::IsParentSelected() const
{
	if (TreeNode* parent = GetParentNode())
	{
		return parent->IsSelected() ? true : parent->IsParentSelected();
	}
	return false;
}