#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include "Globals.h"

class Resource
{
friend class M_Resources;
friend class M_Meshes;
friend class M_Materials;

public:
	enum Type
	{
		MESH,
		TEXTURE,
		BONE,
	};

	Resource(Type type);
	~Resource();

	Type GetType() const;
	unsigned long long GetID() const;

	const char* GetResourceFile() const;

public:
	uint instances;

protected:
	unsigned long long ID;
	Type type;

	std::string resource_file;
	std::string original_file;

};

#endif // !__RESOURCE_H__
