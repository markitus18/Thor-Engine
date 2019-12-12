#include "M_Shaders.h"

M_Shaders::M_Shaders(bool start_enabled) : Module("Shaders", start_enabled)
{

}

M_Shaders::~M_Shaders()
{

}

M_Shaders* M_Shaders::CreateNewShader(const char* assetsPath, uint64 ID)
{

	return nullptr;
}

//Saves both the Assets and the Library file, text and binary
bool M_Shaders::SaveShaderResource(R_Shader* shader)
{

	return false;
}

//Called when we get an external update from the particle system. 
//If it is modified within the engine, both assets and library file are updated, no need to re-import.
void M_Shaders::ImportParticleSystemResource(const char* assetsPath, uint64 ID)
{

}

R_Shader* M_Shaders::LoadShaderResource(uint64 ID)
{

	return nullptr;
}