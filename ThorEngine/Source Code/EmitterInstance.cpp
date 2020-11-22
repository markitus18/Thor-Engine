#include "EmitterInstance.h"

#include "Engine.h"
#include "M_Renderer3D.h"

#include "Emitter.h"
#include "R_Material.h"
#include "ParticleModule.h"

#include <map>

EmitterInstance::EmitterInstance()
{

}

void EmitterInstance::Init(Emitter* emitterReference, C_ParticleSystem* component)
{
	this->emitterReference = emitterReference;
	this->component = component;

	particles.resize(emitterReference->maxParticleCount);
	particleIndices = new unsigned int[emitterReference->maxParticleCount];
	
	for (uint i = 0; i < emitterReference->maxParticleCount; ++i)
	{
		particleIndices[i] = i;
	}
}

void EmitterInstance::Update(float dt)
{
	emitterTime += dt;

	KillDeadParticles();
	UpdateModules(dt);
}

void EmitterInstance::DrawParticles(ERenderingFlags::Flags flags)
{
	for (unsigned int i = 0; i < activeParticles; ++i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		float4x4 transform = float4x4::FromTRS(particle->position, particle->worldRotation, float3(particle->size)).Transposed();
		Engine->renderer3D->AddParticle(transform, emitterReference->hMaterial.Get(), particle->color, particle->distanceToCamera);
	}
}

void EmitterInstance::Reset()
{
	emitterTime = 0.0f;
	KillAllParticles();
}

void EmitterInstance::SpawnParticle()
{
	if (activeParticles == particles.size()) return;		//Ignoring spawn call by now when no more particles are available

	unsigned int particleIndex = particleIndices[activeParticles];
	particles[particleIndex].position = float3::zero;

	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i]->Spawn(this, &particles[particleIndex]);
	}

	++activeParticles;
}

void EmitterInstance::KillDeadParticles()
{
	for (int i = activeParticles - 1; i >= 0; --i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		//Sending the particle to the end of the list (nice unreal trick here)
		if (particle->relativeLifetime >= 1.0f)
		{
			particleIndices[i] = particleIndices[activeParticles - 1]; //We swap the last alive particle with the new dead one
			particleIndices[activeParticles - 1] = particleIndex;
			--activeParticles;
		}
	}
}

void EmitterInstance::KillAllParticles()
{
	activeParticles = 0;
}

void EmitterInstance::UpdateModules(float dt)
{
	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i]->Update(dt, this);
	}
}