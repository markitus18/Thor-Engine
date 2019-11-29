#ifndef __C_PARTICLE_SYSTEM_H__
#define __C_PARTICLE_SYSTEM_H__

#include "Component.h"
#include <vector>

#include "EmitterInstance.h"

class GameObject;

class C_ParticleSystem : public Component
{
public:
	C_ParticleSystem();
	C_ParticleSystem(GameObject* gameObject);
	~C_ParticleSystem();

	static inline Type GetType() { return Type::ParticleSystem; };

	void Update(float dt);
	void Reset();

	void SetResource(Resource* resource);
	void SetResource(unsigned long long id);

public:

private:
	//TODO: can we avoid this necessary include? Vector needs to know particle size
	std::vector<EmitterInstance> emitters;
};

#endif
