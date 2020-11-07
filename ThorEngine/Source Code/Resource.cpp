#include "Resource.h"

Resource::Resource(ResourceType type) : TreeNode(RESOURCE)
{

}

Resource::~Resource()
{
	
}

TreeNode* Resource::GetParentNode() const
{
	return parent;
}

bool Resource::DrawTreeNode() const
{
	return (baseData->type == ResourceType::FOLDER);
}

bool Resource::HasResource(uint64 ID) const
{
	for (uint i = 0; i < baseData->containedResources.size(); ++i)
	{
		if (baseData->containedResources[i] == ID)
			return true;
	}
	return false;
}

void Resource::AddContainedResource(uint64 ID)
{
	if (!HasResource(ID))
		baseData->containedResources.push_back(ID);
}

void Resource::RemoveContainedResource(uint64 ID)
{
	for (uint i = 0; i < baseData->containedResources.size(); ++i)
	{
		if (baseData->containedResources[i] == ID)
			baseData->containedResources.erase(baseData->containedResources.begin() + i);
	}
}

bool Resource::HasContainedResource(uint64 ID) const
{
	for (uint i = 0; i < baseData->containedResources.size(); ++i)
	{
		if (baseData->containedResources[i] == ID)
			return true;
	}
	return false;
}