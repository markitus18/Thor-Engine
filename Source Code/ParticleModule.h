#ifndef __PARTICLE_MODULE_H__
#define __PARTICLE_MODULE_H__

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"

class Particle;
class Emitter;
class EmitterInstance;
struct Config;
#include "MathGeoLib/src/Algorithm/Random/LCG.h"

struct ParticleModule
{
	enum Type
	{
		None,
		EmitterBase,
		EmitterSpawn,
		EmitterArea,
		ParticlePosition,
		ParticleRotation,
		ParticleSize,
		ParticleColor,
		ParticleLifetime,
		ParticleVelocity,
		Unknown
	} type;

	ParticleModule(Type type) : type(type) {};

	virtual void Spawn(EmitterInstance* emitter, Particle* particle) = 0;
	virtual void Update(float dt, EmitterInstance* emitter) = 0;

	virtual void SaveAsset(Config& config);
	virtual void SaveResource(char* buffer);

	virtual void Load(Config& config) = 0;

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
	} alignment = Camera;

	EmitterBase() : ParticleModule(Type::EmitterBase) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float3 emitterOrigin = float3::zero;
	//float3 emitterRotation;
};

struct EmitterSpawn : ParticleModule
{
	EmitterSpawn() : ParticleModule(Type::EmitterSpawn) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float spawnRatio = 0.1f;
	float currentTimer = 0.0f;
};

struct EmitterArea: ParticleModule
{
	EmitterArea() : ParticleModule(Type::EmitterArea) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	//Variable Type? Setting and AABB, Cylinder, point?
};

struct ParticlePosition : ParticleModule
{
	ParticlePosition() : ParticleModule(Type::ParticlePosition) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float3 initialPosition = float3::zero;
};

struct ParticleRotation : ParticleModule
{
	ParticleRotation() : ParticleModule(Type::ParticleRotation) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float initialRotation = 0.0f;
};

struct ParticleSize : ParticleModule
{
	ParticleSize() : ParticleModule(Type::ParticleSize) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float initialSize = 1.0f;
};

struct ParticleColor : ParticleModule
{
	ParticleColor() : ParticleModule(Type::ParticleColor) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float4 initialColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct ParticleLifetime : ParticleModule
{
	ParticleLifetime() : ParticleModule(Type::ParticleLifetime) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float initialLifetime = 3.0f;
};

struct ParticleVelocity : ParticleModule
{
	ParticleVelocity() : ParticleModule(Type::ParticleVelocity) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(float dt, EmitterInstance* emitter);

	void SaveAsset(Config& config);
	void SaveResource(char* buffer);

	void Load(Config& config);

	float4 initialVelocity1 = float4(1.0f, 1.0f, 1.0f, 2.0f);
	float4 initialVelocity2 = float4(-1.0f, 1.0f, -1.0f, 2.0f);

	LCG random;
};

#endif;
