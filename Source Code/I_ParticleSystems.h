#ifndef __M_PARTICLE_SYSTEMS_H__
#define __M_PARTICLE_SYSTEMS_H__

#include "Globals.h"

class R_ParticleSystem;

namespace Importer
{
	namespace Particles
	{
		//Creates an empty particle system resource using default constructor
		R_ParticleSystem* Create();

		//Processes a particle system buffer data into a R_ParticleSystem ready to use
		void Import(const char* buffer, uint size, R_ParticleSystem* rParticles);

		//Process R_Shader data into a buffer ready to save
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 Save(const R_ParticleSystem* rParticles, char** buffer);

		//Process buffer data into a ready-to-use R_Mesh.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_ParticleSystem* rParticles);
	}
}
#endif // !__M_PARTICLE_SYSTEMS_H__

