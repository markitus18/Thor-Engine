#include "EmitterInstance.h"

#include "Application.h"
#include "M_Renderer3D.h"

#include "Emitter.h"
#include "ParticleModule.h"

EmitterInstance::EmitterInstance()
{

}

void EmitterInstance::Init(Emitter* emitterReference, C_ParticleSystem* component)
{
	this->emitterReference = emitterReference;
	this->component = component;
}

void EmitterInstance::Update(float dt)
{
	emitterTime += dt;

	KillDeadParticles();
	UpdateModules(dt);
}

void EmitterInstance::DrawParticles()
{
	for (unsigned int i = 0; i < activeParticles; ++i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		float4x4 transform = float4x4::FromTRS(particle->position, Quat::identity, float3(particle->size)).Transposed();
		App->renderer3D->AddParticle(transform, emitterReference->materialID, particle->color);
	}
}

void EmitterInstance::SpawnParticle()
{
	if (activeParticles = particles.size()) return;		//Ignoring spawn call by now when no more particles are available

	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i].Spawn(this, &particles[activeParticles]);
	}

	++activeParticles;
}

void EmitterInstance::KillDeadParticles()
{
	for (int i = activeParticles; i >= 0; --i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		//Sending the particle to the end of the list (nice unreal trick here)
		if (particle->active == false)
		{
			particleIndices[particleIndex] = particleIndices[activeParticles - 1]; //We swap the last alive particle with the new dead one
			particleIndices[activeParticles - 1] = particleIndex;
			--activeParticles;
		}
	}
}

void EmitterInstance::UpdateModules(float dt)
{
	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i].Update(dt, this);
	}
}