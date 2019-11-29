#include "R_ParticleSystem.h"

#include "Config.h"

#include "ParticleModule.h"

R_ParticleSystem::R_ParticleSystem() : Resource(Resource::Type::PARTICLESYSTEM)
{

}

R_ParticleSystem::~R_ParticleSystem()
{

}

void R_ParticleSystem::InitDefaultSystem()
{
	emitters.clear();
	emitters.push_back(Emitter());

	emitters[0].modules.push_back(new EmitterBase());
	emitters[0].modules.push_back(new EmitterSpawn());
	emitters[0].modules.push_back(new ParticleLifetime());
	emitters[0].modules.push_back(new ParticleSize());
	emitters[0].modules.push_back(new ParticleVelocity());
	emitters[0].modules.push_back(new ParticleColor());
}

void R_ParticleSystem::SaveAsset(Config& config)
{
	config.SetString("Name", name.c_str());
	config.SetString("Original File", original_file.c_str());

	Config_Array emittersArray = config.SetArray("Emitters");

	for (uint i = 0; i < emitters.size(); ++i)
	{
		emitters[i].SaveAsset(emittersArray.AddNode());
	}
}

void R_ParticleSystem::SaveResource(char* buffer)
{

}

void R_ParticleSystem::Load(Config& config)
{
	name = config.GetString("Name");
	original_file = config.GetString("Original File");

	Config_Array emittersArray = config.GetArray("Emitters");

	for (uint i = 0; i < emittersArray.GetSize(); ++i)
	{
		Config emitterNode = emittersArray.GetNode(i);
		emitters.push_back(Emitter());
		emitters[i].Load(emitterNode);
	}

}