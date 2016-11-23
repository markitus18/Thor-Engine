#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>

class Resource
{
public:
	enum Type
	{
		MESH,
		MATERIAL,
		BONE,
	};

	Resource(Type type);
	~Resource();

	Type GetType() const;

private:
	unsigned long long ID;
	Type type;

	std::string resource_file;
	std::string original_file;

};

#endif // !__RESOURCE_H__
