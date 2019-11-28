#include "Particle.h"

Particle::Particle()
{

}

Particle::Particle(const Particle& particle)
{

}

Particle::~Particle()
{

}

void Particle::Update(float dt)
{
	//Security check
	if (active == false) return;

}