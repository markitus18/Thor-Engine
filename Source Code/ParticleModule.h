#ifndef __PARTICLE_MODULE_H__
#define __PARTICLE_MODULE_H__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"

class Particle;
class Emitter;
class EmitterInstance;

struct ParticleModule
{
	ParticleModule() {};
	virtual void Spawn(EmitterInstance* emitter, Particle* particle) {};
	virtual void Update(float dt, EmitterInstance* emitter) {};

	Emitter* emitterContainer = nullptr;
};

struct EmitterBase : ParticleModule
{
	enum Alignment
	{
		None,
		Screen,
		Camera,
		LockYZ,
		LockYX,
		LockXY,
		LockXZ,
		LockZX,
		LockZY,
		Unknown
	} alignment;

	EmitterBase() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float3 emitterOrigin;
	//float3 emitterRotation;

	float spawnRatio = 1.0f;
	float currentTimer = 0.0f;
};

struct EmitterSpawn : ParticleModule
{
	EmitterSpawn() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);
	float spawnRatio = 1.0f;
	float currentTimer = 0.0f;
};

struct EmitterArea: ParticleModule
{
	EmitterArea() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);
	//Variable Type? Setting and AABB, Cylinder, point?
};

struct ParticlePosition : ParticleModule
{
	ParticlePosition() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float3 initialPosition;
};

struct ParticleRotation : ParticleModule
{
	ParticleRotation() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float initialRotation;
};

struct ParticleSize : ParticleModule
{
	ParticleSize() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float initialSize;
};

struct ParticleColor : ParticleModule
{
	ParticleColor() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float4 initialColor;
};

struct ParticleLifetime : ParticleModule
{
	ParticleLifetime() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float initialLifetime;
};

struct ParticleVelocity : ParticleModule
{
	ParticleVelocity() {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	float4 initialVelocity;
};

#endif;
