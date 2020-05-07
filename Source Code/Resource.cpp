#include "Resource.h"

Resource::Resource(Type type) : type(type), TreeNode(RESOURCE)
{

}

Resource::~Resource()
{

}

Resource::Type Resource::GetType() const
{
	return type;
}

unsigned long long Resource::GetID() const
{
	return ID;
}

TreeNode* Resource::GetParentNode() const
{
	return parent;
}

const char* Resource::GetOriginalFile() const
{
	return original_file.c_str();
}

const char* Resource::GetResourceFile() const
{
	return resource_file.c_str();
}

const char* Resource::GetName() const
{
	return name.c_str();
}

bool Resource::DrawTreeNode() const
{
	return (type == FOLDER);
}

bool Resource::HasResource(uint64 ID) const
{
	for (uint i = 0; i < containedResources.size(); ++i)
	{
		if (containedResources[i] == ID)
			return true;
	}
	return false;
}