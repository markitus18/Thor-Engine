#ifndef __RESOURCE_H__
#define __RESOURCE_H__

class Resource
{
	enum Type
	{
		MESH,
		MATERIAL,
		BONE,
	};

public:
	Resource(Type type);
	~Resource();

	Type GetType() const;

private:
	unsigned long long ID;
	Type type;

};

#endif // !__RESOURCE_H__
