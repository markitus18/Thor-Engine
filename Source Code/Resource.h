#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include <vector>

#include "Globals.h"
#include "TreeNode.h"

class Resource : public TreeNode
{

//Huh... can it be done differently?
friend class M_Resources;
friend struct ResourceInfo;
friend class M_ParticleSystems;

public:
	enum Type
	{
		FOLDER,
		MESH,
		TEXTURE,
		MATERIAL,
		ANIMATION,
		ANIMATOR_CONTROLLER,
		MODEL,
		PARTICLESYSTEM,
		SHADER,
		SCENE,
		UNKNOWN,
	};

	Resource(Type type);
	~Resource();

	Type GetType() const;
	unsigned long long GetID() const;
	TreeNode* GetParentNode() const;

	const char* GetOriginalFile() const;
	const char* GetResourceFile() const;
	const char* GetName() const;
	virtual std::vector<TreeNode*> GetChilds() const { std::vector<TreeNode*> ret; return ret; };
	bool IsNodeActive() const { return true; };
	bool DrawTreeNode() const;

	virtual inline void AddContainedResource(uint64 ID) { containedResources.push_back(ID); };

	virtual void LoadOnMemory() {};
	virtual void FreeMemory() {};

public:
	uint instances = 0;
	bool needs_save = false;
	bool isInternal = false;

	//TODO: Set it to prived, moved to public for faster iteration
	std::string name = "";
	std::vector<uint64> containedResources;

protected:
	unsigned long long ID = 0;
	Type type = UNKNOWN;

	std::string resource_file = "";
	std::string original_file = "";

	//TODO: UID that should point to containing folder
	Resource* parent;
};

#endif // !__RESOURCE_H__
