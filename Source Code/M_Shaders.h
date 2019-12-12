#ifndef __M_SHADERS_H__
#define __M_SHADERS_H__

#include "Module.h"

class R_Shader;

class M_Shaders : public Module
{
public:
	M_Shaders(bool start_enabled = true);
	~M_Shaders();

	M_Shaders* CreateNewShader(const char* assetsPath, uint64 ID);

	//Saves both the Assets and the Library file, text and binary
	bool SaveShaderResource(R_Shader* shader);

	//Called when we get an external update from the particle system. 
	//If it is modified within the engine, both assets and library file are updated, no need to re-import.
	void ImportParticleSystemResource(const char* assetsPath, uint64 ID = 0);
	R_Shader* LoadShaderResource(uint64 ID);
};
#endif // !


