#include "ParticleModule.h"

#include "Config.h"

#include "EmitterInstance.h"
#include "C_ParticleSystem.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "Particle.h"
#include "Engine.h"
#include "C_Camera.h"
#include "MathGeoLib/src/Geometry/Frustum.h"

void ParticleModule::Serialize(Config& config)
{
	int intType = (int)type;
	config.Serialize("Type", intType);
}

float3 ParticleModule::RandomFloat3(const float3& a, const float3& b)
{
	return float3(math::Lerp(a.x, b.x, random.Float()), math::Lerp(a.y, b.y, random.Float()), math::Lerp(a.z, b.z, random.Float()));
}

float4 ParticleModule::RandomFloat4(const float4& a, const float4& b)
{
	return float4(math::Lerp(a.x, b.x, random.Float()), math::Lerp(a.y, b.y, random.Float()),
				  math::Lerp(a.z, b.z, random.Float()), math::Lerp(a.w, b.w, random.Float()));
}

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle)
{
	float3 position = emitter->component->gameObject->GetComponent<C_Transform>()->GetPosition();
	particle->position += position + emitterOrigin;
}

void EmitterBase::Update(float dt, EmitterInstance* emitter)
{
	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		//TODO: should this be handled in particle lifetime?
		particle->relativeLifetime += particle->oneOverMaxLifetime * dt;
		//TODO: find a way to link the camera

		//particle->worldRotation = GetAlignmentRotation(particle->position, Engine->camera->GetCamera()->frustum.WorldMatrix());
		
		//TODO: Get scene camera! Maybe through render?
		//particle->distanceToCamera = float3(Engine->camera->GetCamera()->frustum.WorldMatrix().TranslatePart() - particle->position).LengthSq();
	}
}

Quat EmitterBase::GetAlignmentRotation(const float3& position, const float4x4& cameraTransform)
{
	float3 fwd, up, right;
	float3 direction = float3(cameraTransform.TranslatePart() - position).Normalized();

	switch (alignment)
	{
		case(Alignment::Screen):
			fwd = cameraTransform.WorldZ().Normalized().Neg();
			up = cameraTransform.WorldY().Normalized();
			right = up.Cross(fwd).Normalized();
			break;
		case(Alignment::Camera):
			fwd = float3(cameraTransform.TranslatePart() - position).Normalized();
			right = cameraTransform.WorldY().Normalized().Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();
			break;
		case(Alignment::LockXY):
		{
			right = float3::unitX;
			fwd = direction.Cross(right).Normalized();
			up = fwd.Cross(right).Normalized();
			break;
		}
		case(Alignment::LockXZ):
		{
			right = float3::unitX;
			up = direction.Cross(right).Normalized();
			fwd = right.Cross(up).Normalized();
			break;
		}
		case(Alignment::LockYX):
		{
			up = float3::unitY;
			fwd = up.Cross(direction).Normalized();
			right = up.Cross(fwd).Normalized();
			break;
		}
		case(Alignment::LockYZ):
		{
			up = float3::unitY;
			right = up.Cross(direction).Normalized();
			fwd = right.Cross(up).Normalized();
			break;
		}
		case(Alignment::LockZX):
		{
			fwd = float3::unitZ;
			up = direction.Cross(fwd).Normalized();
			right = up.Cross(fwd).Normalized();
			break;
		}
		case(Alignment::LockZY):
		{
			fwd = float3::unitZ;
			right = direction.Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();
			break;
		}
	}
	return Quat(float3x3(right, up, fwd));
}

void EmitterBase::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.SerializeArray("Origin", emitterOrigin.ptr(), 3);

	int intAlignmnet = (int)alignment;
	config.Serialize("Alignment", intAlignmnet);
	alignment = (EmitterBase::Alignment)intAlignmnet;
}

void EmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle)
{

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

void EmitterSpawn::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.Serialize("Spawn Ratio", spawnRatio);
}

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle)
{

}

void EmitterArea::Update(float dt, EmitterInstance* emitter)
{

}

void EmitterArea::Serialize(Config& config)
{
	ParticleModule::Serialize(config);
}

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position += RandomFloat3(initialPosition1, initialPosition2);
}

void ParticlePosition::Update(float dt, EmitterInstance* emitter)
{

}

void ParticlePosition::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.SerializeArray("Initial Position 1", initialPosition1.ptr(), 3);
	config.SerializeArray("Initial Position 2", initialPosition2.ptr(), 3);
}

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->rotation = math::Lerp(initialRotation1, initialRotation2, random.Float());
}

void ParticleRotation::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleRotation::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.Serialize("Initial Rotation 1", initialRotation1);
	config.Serialize("Initial Rotation 2", initialRotation2);
}

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->size = math::Lerp(initialSize1, initialSize2, random.Float());
}

void ParticleSize::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleSize::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.Serialize("Initial Size 1", initialSize1);
	config.Serialize("Initial Size 2", initialSize2);
}

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->color = RandomFloat4(initialColor1, initialColor2);
}

void ParticleColor::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleColor::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.SerializeArray("Initial Color 1", initialColor1.ptr(), 4);
	config.SerializeArray("Initial Color 2", initialColor2.ptr(), 4);
}

void ParticleLifetime::Spawn(EmitterInstance* emitter, Particle* particle)
{
	float lifetime = math::Lerp(initialLifetime1, initialLifetime2, random.Float());
	particle->oneOverMaxLifetime = 1.0f / lifetime;
	particle->relativeLifetime = 0.0f;
}

void ParticleLifetime::Update(float dt, EmitterInstance* emitter)
{
	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->relativeLifetime += particle->oneOverMaxLifetime * dt;
	}
}

void ParticleLifetime::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.Serialize("Initial Lifetime 1", initialLifetime1);
	config.Serialize("Initial Lifetime 2", initialLifetime2);
}

void ParticleVelocity::Spawn(EmitterInstance* emitter, Particle* particle)
{
	float3 direction = float3(math::Lerp(initialVelocity1.x, initialVelocity2.x, random.Float()), math::Lerp(initialVelocity1.y, initialVelocity2.y, random.Float()), math::Lerp(initialVelocity1.z, initialVelocity2.z, random.Float()));
	particle->velocity = float4(direction, initialVelocity1.w);
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

void ParticleVelocity::Serialize(Config& config)
{
	ParticleModule::Serialize(config);

	config.SerializeArray("Initial Velocity 1", initialVelocity1.ptr(), 4);
	config.SerializeArray("Initial Velocity 2", initialVelocity2.ptr(), 4);
}