#ifndef __R_PARTICLE_SYSTEM_H__
#define __R_PARTICLE_SYSTEM_H__

#include "Resource.h"
#include "MathGeoLib/src/Math/float3.h"

class R_ParticleSystem : public Resource
{
public:
	R_ParticleSystem();
	~R_ParticleSystem();

public:
	int particleLifetime = 3.0f;
	float3 particleSpeed = float3(0.0f, 1.0f, 0.0f);
	float spawnRate = 1.0f;

};

#endif
