#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include "ParticleModule.h"
#include "Particle.h"

class Emitter
{
public:
	//Any override constructor we might need?
	Emitter(); 

	//Loop through all modules, loop through particles, update them
	void Update(float dt);

public:
	std::string name;
	std::vector<ParticleModule> modules;
	uint64 materialID;
};

#endif // !__EMITTER_H__

