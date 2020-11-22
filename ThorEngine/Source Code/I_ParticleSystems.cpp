#include "I_ParticleSystems.h"

#include "R_ParticleSystem.h"
#include "Config.h"

R_ParticleSystem* Importer::Particles::Create()
{
	return new R_ParticleSystem();
}

void Importer::Particles::Import(const char* buffer, uint size, R_ParticleSystem* rParticles)
{
	//By now, importing process is the same as loading. Assets and Lib files are the same
	Importer::Particles::Load(buffer, size, rParticles);
}

// TODO: Could be moved to call Resource->Serialize from module resources in a better Config setup
uint64 Importer::Particles::Save(R_ParticleSystem* rParticles, char** buffer)
{
	Config file;
	rParticles->Serialize(file);
	return file.Serialize(buffer);
}

// TODO: Could be moved to call Resource->Serialize from module resources in a better Config setup
void Importer::Particles::Load(const char* buffer, uint size, R_ParticleSystem* rParticles)
{
	Config file(buffer);
	rParticles->Serialize(file);
}