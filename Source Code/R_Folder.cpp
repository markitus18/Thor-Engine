#include "R_Folder.h"

R_Folder::R_Folder() : Resource(Resource::Type::FOLDER)
{

}

R_Folder::~R_Folder()
{

}

std::vector<TreeNode*> R_Folder::GetChilds() const
{
	std::vector<TreeNode*> ret(childs.begin(), childs.end());
	return ret;
}