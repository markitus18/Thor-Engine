#include "M_Shaders.h"

#include "Application.h"
#include "M_FileSystem.h"

#include "R_Shader.h"

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
R_Shader* M_Shaders::ImportShaderResource(const char* assetsPath, uint64 ID)
{
	char* buffer = nullptr;
	R_Shader* newShader = nullptr;

 	uint size = App->fileSystem->Load(assetsPath, &buffer);
	if (size > 0)
	{
		newShader = new R_Shader();
		if (newShader->LoadFromText(buffer))
		{
			std::string full_path("/Library/Shaders/");
			full_path.append(std::to_string(ID));
			newShader->resource_file = full_path;
			newShader->original_file = assetsPath;
			newShader->Link();

		}
		else
		{
			delete newShader;
			newShader = nullptr;
		}
	}
	return newShader;
}

R_Shader* M_Shaders::LoadShaderResource(uint64 ID)
{

	return nullptr;
}