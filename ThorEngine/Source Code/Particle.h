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
	float3 position;			//Current location
	Quat worldRotation;

	float relativeLifetime;		//Lifetime, in range [0, 1]

	float oneOverMaxLifetime;	// = 1.0f / MaxLifeTime
	float rotation;				//Rotation in radians

	float4 velocity;			//Current velocity. Another trick from Unreal, storing speed in w ;)
	float size = 1.0f;

	float4 color;

	float distanceToCamera;
};

#endif // !__PARTICLE_H__

