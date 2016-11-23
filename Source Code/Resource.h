#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>

class Resource
{
friend class M_Resources;

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

protected:
	unsigned long long ID;
	Type type;

	std::string resource_file;
	std::string original_file;

};

#endif // !__RESOURCE_H__
