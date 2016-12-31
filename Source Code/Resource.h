#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include "Globals.h"

class Resource
{
friend class M_Resources;
friend class M_Meshes;
friend class M_Materials;
friend struct ResourceMeta;
friend class M_Import;
friend class M_Animations;

public:
	enum Type
	{
		MESH,
		TEXTURE,
		MATERIAL,
		ANIMATION,
		PREFAB,
		UNKNOWN,
	};

	Resource(Type type);
	~Resource();

	Type GetType() const;
	unsigned long long GetID() const;

	const char* GetOriginalFile() const;
	const char* GetResourceFile() const;
	const char* GetName() const;

	virtual void LoadOnMemory() {};
	virtual void FreeMemory() {};

public:
	uint instances = 0;
	bool needs_save = false;

protected:
	unsigned long long ID = 0;
	Type type = UNKNOWN;

	std::string resource_file = "";
	std::string original_file = "";
	std::string name = "";

};

#endif // !__RESOURCE_H__
