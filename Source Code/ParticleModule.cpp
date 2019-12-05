#include "ParticleModule.h"

#include "Config.h"

#include "EmitterInstance.h"
#include "C_ParticleSystem.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "Particle.h"
#include "Application.h"
#include "M_Camera3D.h"
#include "C_Camera.h"
#include "MathGeoLib/src/Geometry/Frustum.h"

void ParticleModule::SaveAsset(Config& config)
{
	config.SetNumber("Type", (int)type);
}

void ParticleModule::SaveResource(char* buffer)
{
	//TODO: by now it is being saved as json file, same as asset
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
	float3 position = emitter->component->gameObject->GetComponent<C_Transform>()->GetGlobalPosition();
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
		particle->worldRotation = GetAlignmentRotation(particle->position, App->camera->GetCamera()->frustum.WorldMatrix());
		particle->distanceToCamera = float3(App->camera->GetCamera()->frustum.WorldMatrix().TranslatePart() - particle->position).LengthSq();
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

void EmitterBase::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Origin").AddFloat3(emitterOrigin);
	config.SetNumber("Alignment", (int)alignment);
}

void EmitterBase::SaveResource(char* buffer)
{

}

void EmitterBase::Load(Config & config)
{
	emitterOrigin = config.GetArray("Origin").GetFloat3(0);
	alignment = (EmitterBase::Alignment)(int)config.GetNumber("Alignment");
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

void EmitterSpawn::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Spawn Ratio", spawnRatio);
}

void EmitterSpawn::SaveResource(char* buffer)
{

}

void EmitterSpawn::Load(Config & config)
{
	spawnRatio = config.GetNumber("Spawn Ratio");
}

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle)
{

}

void EmitterArea::Update(float dt, EmitterInstance* emitter)
{

}

void EmitterArea::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);
}

void EmitterArea::SaveResource(char* buffer)
{

}

void EmitterArea::Load(Config & config)
{

}

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position += RandomFloat3(initialPosition1, initialPosition2);
}

void ParticlePosition::Update(float dt, EmitterInstance* emitter)
{

}

void ParticlePosition::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Position 1").AddFloat3(initialPosition1);
	config.SetArray("Initial Position 2").AddFloat3(initialPosition2);
}

void ParticlePosition::SaveResource(char* buffer)
{

}

void ParticlePosition::Load(Config & config)
{
	initialPosition1 = config.GetArray("Initial Position 1").GetFloat3(0);
	initialPosition2 = config.GetArray("Initial Position 2").GetFloat3(0);
}

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->rotation = math::Lerp(initialRotation1, initialRotation2, random.Float());
}

void ParticleRotation::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleRotation::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Rotation 1", initialRotation1);
	config.SetNumber("Initial Rotation 2", initialRotation2);
}

void ParticleRotation::SaveResource(char* buffer)
{

}

void ParticleRotation::Load(Config & config)
{
	initialRotation1 = config.GetNumber("Initial Rotation 1");
	initialRotation2 = config.GetNumber("Initial Rotation 2");

}

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->size = math::Lerp(initialSize1, initialSize2, random.Float());
}

void ParticleSize::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleSize::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Size 1", initialSize1);
	config.SetNumber("Initial Size 2", initialSize2);

}

void ParticleSize::SaveResource(char* buffer)
{

}

void ParticleSize::Load(Config & config)
{
	initialSize1 = config.GetNumber("Initial Size 1");
	initialSize2 = config.GetNumber("Initial Size 2");
}

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->color = RandomFloat4(initialColor1, initialColor2);
}

void ParticleColor::Update(float dt, EmitterInstance* emitter)
{

}

void ParticleColor::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Color 1").AddFloat4(initialColor1);
	config.SetArray("Initial Color 2").AddFloat4(initialColor2);
}

void ParticleColor::SaveResource(char* buffer)
{

}

void ParticleColor::Load(Config & config)
{
	initialColor1 = config.GetArray("Initial Color 1").GetFloat4(0);
	initialColor2 = config.GetArray("Initial Color 2").GetFloat4(0);
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

void ParticleLifetime::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetNumber("Initial Lifetime 1", initialLifetime1);
	config.SetNumber("Initial Lifetime 2", initialLifetime2);
}

void ParticleLifetime::SaveResource(char* buffer)
{

}

void ParticleLifetime::Load(Config & config)
{
	initialLifetime1 = config.GetNumber("Initial Lifetime 1");
	initialLifetime2 = config.GetNumber("Initial Lifetime 2");
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

void ParticleVelocity::SaveAsset(Config& config)
{
	ParticleModule::SaveAsset(config);

	config.SetArray("Initial Velocity 1").AddFloat4(initialVelocity1);
	config.SetArray("Initial Velocity 2").AddFloat4(initialVelocity2);
}

void ParticleVelocity::SaveResource(char* buffer)
{

}

void ParticleVelocity::Load(Config & config)
{
	initialVelocity1 = config.GetArray("Initial Velocity 1").GetFloat4(0);
	initialVelocity2 = config.GetArray("Initial Velocity 2").GetFloat4(0);

}
