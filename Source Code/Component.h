#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

//TODO: move into .cpp?
#include "MathGeoLib\src\MathGeoLib.h"

class GameObject;
class Resource;

class Component
{
public:
	enum Type
	{
		None,
		Transform,
		Mesh,
		Material,
		Camera,
		Animation,
	};

public:
	Component(Type type, GameObject*, bool hasResource);
	virtual ~Component();

	void SetActive(bool set);
	bool IsActive() const;
	Type GetType() const;

	virtual void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	virtual void Save();
	virtual void Load();

	//Resource handling
	void SetResource(Resource* resource);
	void SetResource(unsigned long long id);
	
	Resource* GetResource();
	const Resource* GetResource() const;
	bool HasResource() const;

public:
	GameObject* gameObject;

protected:
	bool hasResource = false;
	bool active = true;
	Type type = None;
	uint64 resourceID = 0;


};
#endif //__COMPONENT_H__
