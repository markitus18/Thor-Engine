#ifndef __C_PARTICLE_SYSTEM_H__
#define __C_PARTICLE_SYSTEM_H__

#include "Component.h"
#include <vector>

#include "EmitterInstance.h"

class GameObject;
class R_ParticleSystem;

class C_ParticleSystem : public Component
{
public:
	C_ParticleSystem();
	C_ParticleSystem(GameObject* gameObject);
	~C_ParticleSystem();

	static inline Type GetType() { return Type::ParticleSystem; };

	void Update();
	void Reset();

	virtual void Draw(ERenderingFlags::Flags flags) override;
	void Serialize(Config& config) override;

	void SetResource(Resource* resource);
	void SetResource(unsigned long long id);
	
	uint64 GetResourceID() const;

public:
	//TODO: can we avoid this necessary include? Vector needs to know instances size
	std::vector<EmitterInstance> emitters;

	ResourceHandle<R_ParticleSystem> rParticleSystemHandle;
};

#endif
