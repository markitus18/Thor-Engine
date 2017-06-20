#ifndef __SELECTABLE_H__
#define __SELECTABLE_H__

#include <vector>
#include <string>

enum TreeNode_Type
{
	GAMEOBJECT,
	RESOURCE,
};

template <class Type>
class TreeNode
{
public:
	TreeNode(Type* data, TreeNode_Type type)
	{
		this->data = data;
		this->type = type;
	}

	Type* GetData() const { return data; }
	TreeNode_Type GetType() const { return type; }

	std::vector<TreeNode*> GetChilds();
	const char* GetName() const;
	
private:
	Type* data;
	TreeNode_Type type;
};

#endif // !__SELECTABLE_H__
