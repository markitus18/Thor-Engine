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
	};

public:
	Component(Type type, const GameObject*);
	virtual ~Component();

	void SetActive(bool set);
	bool IsActive() const;
	Type GetType() const;

	virtual void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	virtual void Save();
	virtual void Load();

//Resource handling
	void SetResource(Resource* resource);
	//Made only for external file importing, it does not set the pointer
	void SetResource(unsigned long long id);
	
	Resource* GetResource();
	const Resource* GetResource() const;

public:
	const GameObject* gameObject;

protected:
	bool active;
	Type type;
	unsigned long long resourceID;
	Resource* resource = nullptr;


};
#endif //__COMPONENT_H__
