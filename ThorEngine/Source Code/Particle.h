#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/Quat.h"

class Particle
{
public:
	Particle();
	Particle(const Particle& particle);
	~Particle();

	void Update(float dt);

public:
	float3 position = float3::zero;			//Current location
	Quat worldRotation = Quat::identity;

	float relativeLifetime = 0.0f;			//Lifetime, in range [0, 1]

	float oneOverMaxLifetime = 1.0f;		// = 1.0f / MaxLifeTime
	float rotation = 0.0f;					//Rotation in radians

	float4 velocity = float4::zero;			//Current velocity. Another trick from Unreal, storing speed in w ;)
	float size = 1.0f;

	float4 color = float4::one;

	float distanceToCamera;
};

#endif // !__PARTICLE_H__

