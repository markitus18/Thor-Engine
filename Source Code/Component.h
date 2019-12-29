#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

//TODO: move into .cpp?
#include "MathGeoLib/src/Math/float4x4.h"

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
		Bone,
		Billboard,
		ParticleSystem,
		Unknown
	};

public:
	Component(Type type, GameObject*, bool hasResource);
	virtual ~Component();

	void SetActive(bool set);
	inline bool IsActive() const { return active; };
	inline Type GetType() const { return type; };

	virtual void Update(float dt) {}; //TODO: should return bool?
	virtual void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	//Resource handling
	virtual void SetResource(Resource* resource);
	virtual void SetResource(unsigned long long id, bool load = true);
	
	Resource* GetResource();
	const Resource* GetResource() const;
	inline uint64 GetResourceID() const { return resourceID; };
	inline bool HasResource() const { return hasResource; };

public:
	GameObject* gameObject = nullptr;

protected:
	bool hasResource = false;
	bool active = true;
	Type type = None;
	uint64 resourceID = 0;


};
#endif //__COMPONENT_H__
