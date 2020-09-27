#ifndef __RESOURCE_HANDLE_H__
#define __RESOURCE_HANDLE_H__

#include "Resource.h"

typedef unsigned __int64 uint64;

template <typename T = Resource>
class ResourceHandle
{
public:
	ResourceHandle() {};
	ResourceHandle(uint64 ID) : ID(ID) { }
	ResourceHandle(T* resource) : resource(resource)
	{
		resource->instances++;
		ID = resource->GetID();
	}

	~ResourceHandle()
	{
		Free();
	}

	void Set(uint64 ID)
	{
		if (resource && resource->GetID() != ID)
			Free();

		if (!resource)
			this->ID = ID;
	}

	void Set(T* resource)
	{
		if (this->resource && this->resource != resource)
			Free();

		if (!this->resource)
		{
			this->resource = resource;
			resource->instances++;
			this->ID = resource->GetID();
		}
	}

	//Loads the resource if it has not been loaded previously. Compiler throws errors when trying 'inline'
	inline T* Get() { return resource ? resource : (resource = RequestResource()); }
	inline const T* Get() const { return resource ? resource : (resource = RequestResource()); }

	//Releases the use of the resource in module resources
	void Free();

	inline uint64 GetID() const { return ID; }
	inline bool IsLoaded() const { return resource != nullptr; };

private:
	T* RequestResource() const;

private:
	uint64 ID = 0;
	mutable T* resource = nullptr;
};

#endif //__RESOURCE_HANDLE_H__
