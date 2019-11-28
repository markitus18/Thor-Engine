#include "ParticleModule.h"

#include "EmitterInstance.h"
#include "Particle.h"

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle)
{

}

void EmitterBase::Update(float dt, EmitterInstance* emitter)
{

}

void EmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->active = true;
}

void EmitterSpawn::Update(float dt, EmitterInstance* emitter)
{
	currentTimer += dt;
	if (currentTimer > spawnRatio)
	{
		currentTimer -= spawnRatio;
		emitter->SpawnParticle();
	}
}

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle)
{

}

void EmitterArea::Update(float dt, EmitterInstance* emitter)
{

}

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position = initialPosition;
}

void ParticlePosition::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->rotation = initialRotation;
}

void ParticleRotation::Update(float dt, EmitterInstance* emitter)

{

}

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->size = initialSize;
}

void ParticleSize::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->color = initialColor;
}

void ParticleColor::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleLifetime::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->oneOverMaxLifetime = 1.0f / initialLifetime;
	particle->relativeLifetime = 0.0f;
}

void ParticleLifetime::Update(float dt, EmitterInstance* emitter)
{
	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->relativeLifetime += particle->oneOverMaxLifetime *= dt;
	}
}

void ParticleVelocity::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->velocity = initialVelocity;
}

void ParticleVelocity::Update(float dt, EmitterInstance* emitter)

{

}