#include "Emitter.h"

#include "Config.h"

#include "ParticleModule.h"

Emitter::Emitter()
{

}

void Emitter::Update(float dt)
{

}

void Emitter::SaveAsset(Config& config) const
{
	config.SetString("Name", name.c_str());
	config.SetNumber("Material ID", materialID);
	config.SetNumber("Max Particle Count", maxParticleCount);

	Config_Array modulesArray = config.SetArray("Modules");

	for (uint i = 0; i < modules.size(); ++i)
	{
		modules[i]->SaveAsset(modulesArray.AddNode());
	}
}

void Emitter::SaveResource(char* buffer)
{

}

void Emitter::Load(Config& config)
{
	name = config.GetString("Name");
	materialID = config.GetNumber("Material ID", 0);
	maxParticleCount = config.GetNumber("Max Particle Count");

	Config_Array modulesArray = config.GetArray("Modules");

	for (uint i = 0; i < modulesArray.GetSize(); ++i)
	{
		Config modulesArraNode = modulesArray.GetNode(i);

		if (AddModuleFromType((ParticleModule::Type)(int)modulesArraNode.GetNumber("Type")))
			modules.back()->Load(modulesArraNode);
	}

}

bool Emitter::AddModuleFromType(ParticleModule::Type type)
{
	switch (type)
	{
		case(ParticleModule::Type::EmitterBase):
		{
			modules.push_back(new EmitterBase());
			return true;
		}
		case(ParticleModule::Type::EmitterSpawn):
		{
			modules.push_back(new EmitterSpawn());
			return true;
		}
		case(ParticleModule::Type::EmitterArea):
		{
			modules.push_back(new EmitterArea());
			return true;
		}
		case(ParticleModule::Type::ParticlePosition):
		{
			modules.push_back(new ParticlePosition());
			return true;
		}
		case(ParticleModule::Type::ParticleRotation):
		{
			modules.push_back(new ParticleRotation());
			return true;
		}
		case(ParticleModule::Type::ParticleSize):
		{
			modules.push_back(new ParticleSize());
			return true;
		}
		case(ParticleModule::Type::ParticleColor):
		{
			modules.push_back(new ParticleColor());
			return true;
		}
		case(ParticleModule::Type::ParticleLifetime):
		{
			modules.push_back(new ParticleLifetime());
			return true;
		}
		case(ParticleModule::Type::ParticleVelocity):
		{
			modules.push_back(new ParticleVelocity());
			return true;
		}
	}
	return false;
}