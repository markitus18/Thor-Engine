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
			newShader->ID = ID;
			newShader->Link();

			//TODO: simply duplicating shader in library, not saving it as binary yet
			uint ret = App->fileSystem->Save(newShader->resource_file.c_str(), buffer, size);
			/*
			char* binaryShader = nullptr;
			if (int size = newShader->Save(&binaryShader))
			{
				uint ret = App->fileSystem->Save(newShader->resource_file.c_str(), binaryShader, size);
				RELEASE_ARRAY(binaryShader);
			}*/
			
		}
		else
		{
			RELEASE(newShader);
		}
		RELEASE_ARRAY(buffer);
	}
	return newShader;
}

R_Shader* M_Shaders::LoadShaderResource(uint64 ID)
{
	R_Shader* loadedShader = nullptr;

	std::string full_path = "/Library/Shaders/";
	full_path.append(std::to_string(ID));

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		//TODO: simply duplicating shader in library, not saving it as binary yet.
		//Read method from library is the same as assets
		loadedShader = new R_Shader();
		if (loadedShader->LoadFromText(buffer) == false)
		{
			RELEASE(loadedShader);
		}
		else
		{
			loadedShader->Link();
		}
		/*
		if (loadedShader->LoadFromBinary(buffer, size) == false)
		{
			LOG("Could not load shader from binary file: %s", full_path.c_str());
			RELEASE(loadedShader);
		}
		RELEASE_ARRAY(buffer);
		*/
	}
	return loadedShader;
}