#include "I_ParticleSystems.h"

#include "R_ParticleSystem.h"
#include "Config.h"

R_ParticleSystem* Importer::Particles::Create()
{
	return new R_ParticleSystem();
}

void Importer::Particles::Import(const char* buffer, uint size, R_ParticleSystem* rParticles)
{
	//By now, importing process is the same as loding. Assets and Lib files are the same
	Importer::Particles::Load(buffer, size, rParticles);
}

uint64 Importer::Particles::Save(const R_ParticleSystem* rParticles, char** buffer)
{
	Config file;
	rParticles->SaveAsset(file);
	return file.Serialize(buffer);
}

void Importer::Particles::Load(const char* buffer, uint size, R_ParticleSystem* rParticles)
{
	Config file(buffer);
	rParticles->Load(file);
}