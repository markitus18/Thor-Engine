#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__

#include "Particle.h"

class Emitter;
class C_ParticleSystem;

class EmitterInstance
{
public:
	EmitterInstance();

	void Init(Emitter* emitterReference, C_ParticleSystem* component);

	void Update(float dt);
	void DrawParticles();
	void Reset();

	void SpawnParticle();

private:
	void KillDeadParticles();
	void KillAllParticles();
	void UpdateModules(float dt);

public:
	float emitterTime = 0;					//Keeping emitter time count, useful later for looping

	//Particle-related data
	unsigned int activeParticles = 0;
	unsigned int* particleIndices;		//Ha ha, nice trick learnt from unreal, this is pure fun.
	std::vector<Particle> particles;

	Emitter* emitterReference;			//A reference to the emitter resource
	C_ParticleSystem* component;		//A reference to the particle system component
};

#endif 
