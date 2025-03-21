#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "ResourceHandle.h"
#include "ERenderingFlagsh.h"

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
		Animator,
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

	virtual void Update() {};
	virtual void OnTransformUpdated() {};

	virtual void Draw(ERenderingFlags::Flags flags) { }

	virtual void Serialize(Config& config);

	//Resource handling
	//virtual void SetResource(Resource* resource) { }
	//virtual void SetResource(unsigned long long id) { }
	
	//virtual uint64 GetResourceID() const { return 0; }
	//inline bool HasResource() const { return hasResource; }

public:
	GameObject* gameObject = nullptr;

protected:
	bool hasResource = false;
	bool active = true;
	Type type = None;
};
#endif //__COMPONENT_H__
