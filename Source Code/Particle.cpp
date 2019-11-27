#include "Particle.h"

#include "Application.h"
#include "M_Renderer3D.h"

Particle::Particle() : transform(nullptr)
{

}

Particle::Particle(const Particle& particle) : transform(nullptr), lifetime(particle.lifetime), speed(particle.speed), materialID(particle.materialID)
{

}

Particle::~Particle()
{

}

void Particle::Update(float dt)
{
	//Security check
	if (active == false) return;

	lifetime -= dt;
	if (lifetime <= .0f)
	{
		active = false;
		return;
	}
}

void Particle::Draw()
{
	App->renderer3D->AddParticle(transform.GetGlobalTransformT(), materialID);
}

void Particle::SetMaterial()
{

}