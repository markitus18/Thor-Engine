#include "TreeNode.h"

uint TreeNode::GetChildNodeIndex(const TreeNode* child) const
{
	int count = 0;
	std::vector<TreeNode*>::const_iterator it;
	std::vector<TreeNode*> childs = GetChilds();
	if (childs.size() > 0)
	{
		for (it = childs.begin(); it != childs.end(); ++it)
		{
			if ((*it) == child)
			{
				return count;
			}
			++count;
		}
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
			if (TreeNode* ret = parent->GetChildNode(++childIndex))
				return ret;
		}
		toEvaluate = parent;
		parent = parent->GetParentNode();
	}
	return nullptr;
}

TreeNode* TreeNode::GetPreviousOpenNode() const
{
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
		TreeNode* parent = GetParentNode();
		return parent->GetID() != 0 ? parent : nullptr;
	}
}

TreeNode* TreeNode::GetFirstOpenNode(TreeNode* root, TreeNode* first, TreeNode* second)
{
	if (first == second) return first;

	TreeNode* toEvaluate = root->GetNextOpenNode();
	while (toEvaluate != nullptr)
	{
		if (first == toEvaluate) return first;
		if (second == toEvaluate) return second;
		toEvaluate = toEvaluate->GetNextOpenNode();
	}
	return nullptr;
}

std::vector<TreeNode*>::const_iterator TreeNode::GetFirstOpenNode(TreeNode* root, const std::vector<TreeNode*>& vector)
{
	TreeNode* toEvaluate = root->GetNextOpenNode();
	while (toEvaluate != nullptr)
	{
		std::vector<TreeNode*>::const_iterator it;
		for (it = vector.begin(); it != vector.end(); ++it)
		{
			if (*it == toEvaluate)
				return it;
		}
		toEvaluate = toEvaluate->GetNextOpenNode();
	}
	return vector.end();
}

bool TreeNode::HasChildNodeInTree(TreeNode* child) const
{
	std::vector<TreeNode*>::const_iterator it;
	std::vector<TreeNode*> childs = GetChilds();
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if (*it == child)
			return true;
	}
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it)->HasChildNodeInTree(child))
			return true;
	}
	return false;
}

void TreeNode::RecalculateOpenNodes()
{
	std::vector<TreeNode*> childs = GetChilds();
	if (hierarchyOpen == true)
	{
		if (childs.size() > 0)
			beenSelected = true;
		else
			hierarchyOpen = false;
	}

	std::vector<TreeNode*>::const_iterator it;
	for (it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->RecalculateOpenNodes();
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