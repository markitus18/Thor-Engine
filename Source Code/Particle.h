#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"

#include "C_Transform.h"

#include "MathGeoLib/src/Math/float3.h"


class Particle
{
public:
	Particle();
	Particle(const Particle& particle);
	~Particle();

	void Update(float dt);
	void Draw();
	void SetMaterial();

	inline bool IsActive() const { return active; };

private:
	float lifetime = 3.0f;
	float3 speed = float3::zero;
	uint64 materialID = 0;

	//Yup... we are going to store position data in a transform (can it be done differently?)
	//Let's try to fake the transform system.
	C_Transform transform;
	bool active = false;
};

#endif // !__PARTICLE_H__

