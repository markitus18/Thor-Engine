#include "M_ParticleSystems.h"

#include "Application.h"
#include "Config.h"

#include "M_FileSystem.h"

#include "ParticleModule.h"
#include "R_ParticleSystem.h"

M_ParticleSystems::M_ParticleSystems(bool start_enabled) : Module("Particles", start_enabled)
{

}

M_ParticleSystems::~M_ParticleSystems()
{

}

R_ParticleSystem* M_ParticleSystems::CreateNewParticleSystem(const char* assetsPath, uint64 ID)
{
	R_ParticleSystem* particleSystem = new R_ParticleSystem;
	particleSystem->original_file = std::string(assetsPath) + "/" + "NewParticleSystem.particles";
	particleSystem->name = "NewParticleSystem";
	particleSystem->resource_file = std::string("/Library/ParticleSystems/") + std::to_string(ID);
	particleSystem->ID = ID;

	particleSystem->InitDefaultSystem();

	SaveParticleSystemResource(particleSystem);

	return particleSystem;
}

bool M_ParticleSystems::SaveParticleSystemResource(R_ParticleSystem* particleSystem)
{
	Config assetFile;
	particleSystem->SaveAsset(assetFile);

	char* buffer = nullptr;
	uint size = assetFile.Serialize(&buffer);

	if (size > 0)
	{
		//Saving the same file in both assets and library by now
		App->fileSystem->Save(particleSystem->original_file.c_str(), buffer, size);
		App->fileSystem->Save(particleSystem->resource_file.c_str(), buffer, size);
		delete buffer;

		return true;
	}
	return false;
}

void M_ParticleSystems::ImportParticleSystemResource(const char* assetsPath, uint64 ID)
{
	char* buffer = nullptr;
	uint size = App->fileSystem->Load(assetsPath, &buffer);
	if (size > 0)
	{
		std::string path = std::string("Library/ParticleSystems/") + std::to_string(ID);
		App->fileSystem->Save(path.c_str(), buffer, size);
	}
}

R_ParticleSystem* M_ParticleSystems::LoadParticleSystemResource(uint64 ID)
{
	char* buffer = nullptr;
	std::string path = std::string("Library/ParticleSystems/") + std::to_string(ID);

	uint size = App->fileSystem->Load(path.c_str(), &buffer);

	if (size > 0)
	{
		Config config(buffer);

		R_ParticleSystem* particleSystem = new R_ParticleSystem();
		particleSystem->Load(config);
		particleSystem->ID = ID;
		particleSystem->resource_file = path;

		delete buffer;
		return particleSystem;
	}
}