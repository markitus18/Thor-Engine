#include "R_ParticleSystem.h"

#include "Config.h"

#include "ParticleModule.h"

R_ParticleSystem::R_ParticleSystem() : Resource(ResourceType::PARTICLESYSTEM)
{
	emitters.reserve(4);
}

R_ParticleSystem::~R_ParticleSystem()
{

}

void R_ParticleSystem::InitDefaultSystem()
{
	emitters.clear();
	emitters.reserve(4);
	AddDefaultEmitter();
}

void R_ParticleSystem::AddDefaultEmitter()
{
	emitters.push_back(Emitter());

	emitters.back().modules.push_back(new EmitterBase());
	emitters.back().modules.push_back(new EmitterSpawn());
	emitters.back().modules.push_back(new ParticleLifetime());
	emitters.back().modules.push_back(new ParticleVelocity());
	emitters.back().modules.push_back(new ParticleSize());
	emitters.back().modules.push_back(new ParticleColor());
}

void R_ParticleSystem::Serialize(Config& config)
{
	config.SerializeVector("Emitters", emitters);
}