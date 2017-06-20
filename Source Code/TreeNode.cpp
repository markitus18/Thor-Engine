#include "TreeNode.h"
#include "GameObject.h"
#include "Resource.h"
#include "R_Folder.h"

//template class TreeNode<GameObject>;
//template class TreeNode<Resource>;

template <class Type>
std::vector<TreeNode<Type>*> TreeNode<Type>::GetChilds()
{	
	if (type == GAMEOBJECT)
	{
		GameObject* go = (GameObject*)data;
		std::vector<TreeNode<Type>*> ret(go->GetChilds().begin(), go->GetChilds().end());
		return ret;
	}
	else if (type == RESOURCE)
	{
		if (data->GetType() == Resource::FOLDER)
		{
			 R_Folder* folder = (R_Folder*)data;
			 std::vector<TreeNode<Type>*> ret(folder->GetChilds().begin(), folder->GetChilds().end());
			 return ret;
		}
	}
	std::vector<TreeNode<Type>*> ret;
	return ret;
}

template <class Type>
const char* TreeNode<Type>::GetName() const
{
	return data->GetName();
}


