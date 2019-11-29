#include "ParticleModule.h"

#include "Config.h"

#include "EmitterInstance.h"
#include "C_ParticleSystem.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "Particle.h"

void ParticleModule::SaveAsset(Config& config)
{
	config.SetNumber("Type", (int)type);
}

void ParticleModule::SaveResource(char* buffer)
{
	//TODO: by now it is being saved as json file, same as asset
}

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position += emitter->component->gameObject->GetComponent<C_Transform>()->GetGlobalPosition();
}

void EmitterBase::Update(float dt, EmitterInstance* emitter)
{

}

void EmitterBase::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Origin").AddFloat3(emitterOrigin);
	config.SetNumber("Alignment", (int)alignment);
}

void EmitterBase::SaveResource(char* buffer)
{

}

void EmitterBase::Load(Config & config)
{
	emitterOrigin = config.GetArray("Origin").GetFloat3(0);
	alignment = (EmitterBase::Alignment)(int)config.GetNumber("Alignment");
}

void EmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->active = true;
	particle->position = float3(.0f, .0f, .0f);
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

void EmitterSpawn::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Spawn Ratio", spawnRatio);
}

void EmitterSpawn::SaveResource(char* buffer)
{

}

void EmitterSpawn::Load(Config & config)
{
	spawnRatio = config.GetNumber("Spawn Ratio");
}

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle)
{

}

void EmitterArea::Update(float dt, EmitterInstance* emitter)
{

}

void EmitterArea::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);
}

void EmitterArea::SaveResource(char* buffer)
{

}

void EmitterArea::Load(Config & config)
{

}

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position += initialPosition;
}

void ParticlePosition::Update(float dt, EmitterInstance* emitter)
{

}

void ParticlePosition::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Position").AddFloat3(initialPosition);
}

void ParticlePosition::SaveResource(char* buffer)
{

}

void ParticlePosition::Load(Config & config)
{
	initialPosition = config.GetArray("Initial Position").GetFloat3(0);
}

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->rotation = initialRotation;
}

void ParticleRotation::Update(float dt, EmitterInstance* emitter)

{

}

void ParticleRotation::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Rotation", initialRotation);
}

void ParticleRotation::SaveResource(char* buffer)
{

}

void ParticleRotation::Load(Config & config)
{
	initialRotation = config.GetNumber("Initial Rotation");
}

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->size = initialSize;
}

void ParticleSize::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleSize::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Size", initialSize);
}

void ParticleSize::SaveResource(char* buffer)
{

}

void ParticleSize::Load(Config & config)
{
	initialSize = config.GetNumber("Initial Size");
}

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->color = initialColor;
}

void ParticleColor::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleColor::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Color").AddFloat4(initialColor);
}

void ParticleColor::SaveResource(char* buffer)
{

}

void ParticleColor::Load(Config & config)
{
	initialColor = config.GetArray("Initial Color").GetFloat4(0);
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

void ParticleLifetime::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Lifetime", initialLifetime);
}

void ParticleLifetime::SaveResource(char* buffer)
{

}

void ParticleLifetime::Load(Config & config)
{
	initialLifetime = config.GetNumber("Initial Lifetime");
}

void ParticleVelocity::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->velocity = initialVelocity;
}

void ParticleVelocity::Update(float dt, EmitterInstance* emitter)
{
	//TODO: should not be here, will be moved later
	for (int i = emitter->activeParticles - 1; i >= 0; --i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->position += particle->velocity.Float3Part() * particle->velocity.w * dt;
	}
}

void ParticleVelocity::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Velocity").AddFloat4(initialVelocity);
}

void ParticleVelocity::SaveResource(char* buffer)
{

}

void ParticleVelocity::Load(Config & config)
{
	initialVelocity = config.GetArray("Initial Velocity").GetFloat4(0);
}
