#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include "Particle.h"
#include "ParticleModule.h"
#include "ResourceHandle.h"

class R_Material;

class Emitter
{
public:
	//Any override constructor we might need?
	Emitter(); 

	//Loop through all modules, loop through particles, update them
	void Update(float dt);

	void Serialize(Config& config);

	ParticleModule* AddModuleFromType(ParticleModule::Type type);

public:
	std::string name = "Particle Emitter";
	std::vector<ParticleModule*> modules;
	ResourceHandle<R_Material> hMaterial; //TODO: Hardcode default bubble ID!!
	
	int maxParticleCount = 100;
};

#endif // !__EMITTER_H__

