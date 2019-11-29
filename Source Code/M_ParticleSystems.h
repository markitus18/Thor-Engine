#ifndef __M_PARTICLE_SYSTEMS_H__
#define __M_PARTICLE_SYSTEMS_H__

#include "Module.h"

class R_ParticleSystem;
class Emitter;
class ParticleModule;

class M_ParticleSystems : public Module
{
public:
	M_ParticleSystems(bool start_enabled = true);
	~M_ParticleSystems();

	R_ParticleSystem* CreateNewParticleSystem(const char* assetsPath, uint64 ID);

	//Saves both the Assets and the Library file, json and binary (not yet binary)
	bool SaveParticleSystemResource(R_ParticleSystem* particleSystem);

	//Called when we get an external update from the particle system. 
	//If it is modified within the engine, both assets and library file are updated, no need to re-import.
	void ImportParticleSystemResource(const char* assetsPath, uint64 ID = 0);
	R_ParticleSystem* LoadParticleSystemResource(uint64 ID);

};

#endif // !__M_PARTICLE_SYSTEMS_H__

