#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include <vector>

#include "TreeNode.h"
#include "ResourceBase.h"

class Resource : public TreeNode
{
	//Huh... can it be done differently?
	friend class M_Resources;
	friend class M_ParticleSystems;

public:

	Resource(ResourceType type);
	~Resource();

	TreeNode* GetParentNode() const;

	//Getter functions for retrieving base data
	inline ResourceType GetType() const { return baseData->type; }
	inline unsigned long long GetID() const { return baseData->ID; }
	inline const char* GetAssetsFile() const { return baseData->assetsFile.c_str(); }
	inline const char* GetLibraryFile() const { return baseData->libraryFile.c_str(); }
	inline const char* GetName() const { return baseData->name.c_str(); }

	virtual std::vector<TreeNode*> GetChilds() const { std::vector<TreeNode*> ret; return ret; };
	bool IsNodeActive() const { return true; };
	bool DrawTreeNode() const;
	bool HasResource(uint64 ID) const;

	virtual void AddContainedResource(uint64 ID);
	virtual void RemoveContainedResource(uint64 ID);

	virtual void LoadOnMemory() {};
	virtual void FreeMemory() {};

public:
	uint instances = 0;
	bool needs_save = false;
	bool isExternal = false;

public: //TODO: Can be set to private? Importers need to modify its content
	struct ResourceBase* baseData = nullptr;

	//TODO: UID that should point to containing folder
	Resource* parent = nullptr;
};  

#endif // !__RESOURCE_H__
