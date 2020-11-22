#include "Emitter.h"

#include "Config.h"

#include "R_Material.h"
#include "ParticleModule.h"

Emitter::Emitter()
{

}

void Emitter::Update(float dt)
{

}

void Emitter::Serialize(Config& config)
{
	config.Serialize("Name", name);
	hMaterial.Serialize("Material ID", config);
	config.Serialize("Max Particle Count", maxParticleCount);

	if (config.isSaving)
	{
		Config_Array arr = config.GetArray("Modules");
		for (uint i = 0; i < modules.size(); ++i)
		{
			modules[i]->Serialize(arr.GetNode(i));
		}
	}
	else
	{
		Config_Array arr = config.GetArray("Modules");
		for (uint i = 0; i < arr.GetSize(); ++i)
		{
			int nodeType = 0;
			arr.GetNode(i).Serialize("Type", nodeType);

			if (ParticleModule* newModule = AddModuleFromType((ParticleModule::Type)nodeType))
				newModule->Serialize(arr.GetNode(i));
		}
	}
}

ParticleModule* Emitter::AddModuleFromType(ParticleModule::Type type)
{
	switch (type)
	{
		case(ParticleModule::Type::EmitterBase):
		{
			modules.push_back(new EmitterBase());
			return modules.back();
		}
		case(ParticleModule::Type::EmitterSpawn):
		{
			modules.push_back(new EmitterSpawn());
			return modules.back();
		}
		case(ParticleModule::Type::EmitterArea):
		{
			modules.push_back(new EmitterArea());
			return modules.back();
		}
		case(ParticleModule::Type::ParticlePosition):
		{
			modules.push_back(new ParticlePosition());
			return modules.back();
		}
		case(ParticleModule::Type::ParticleRotation):
		{
			modules.push_back(new ParticleRotation());
			return modules.back();
		}
		case(ParticleModule::Type::ParticleSize):
		{
			modules.push_back(new ParticleSize());
			return modules.back();
		}
		case(ParticleModule::Type::ParticleColor):
		{
			modules.push_back(new ParticleColor());
			return modules.back();
		}
		case(ParticleModule::Type::ParticleLifetime):
		{
			modules.push_back(new ParticleLifetime());
			return modules.back();
		}
		case(ParticleModule::Type::ParticleVelocity):
		{
			modules.push_back(new ParticleVelocity());
			return modules.back();
		}
	}
	return nullptr;
}