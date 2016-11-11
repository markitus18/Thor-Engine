#include "ModuleMaterials.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "C_Material.h"
#include "Color.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )



ModuleMaterials::ModuleMaterials(Application* app, bool start_enabled) : Module("Materials", start_enabled)
{

}

ModuleMaterials::~ModuleMaterials()
{

}

bool ModuleMaterials::Init()
{
	return true;
}

bool ModuleMaterials::CleanUp()
{
	return true;
}

/*Search for a material with same texture path
if a material is found, returns the material
otherwise returns nullptr*/
C_Material* ModuleMaterials::Exists(const char* texture_path) const
{
	std::list<C_Material*>::const_iterator it = materials.begin();
	while (it != materials.end())
	{
		if ((*it)->texture_path == texture_path)
			return (*it);
		it++;
	}
	return nullptr;
}

C_Material* ModuleMaterials::ImportMaterial(const aiMaterial* from, const std::string& path)
{
	uint numTextures = from->GetTextureCount(aiTextureType_DIFFUSE);
	std::string texture_file = "", texture_extension = "";
	std::string texture_path;
	if (numTextures > 0)
	{
		aiString aiStr;
		aiReturn ret = from->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
		App->fileSystem->SplitFilePath(aiStr.C_Str(), nullptr, &texture_file, &texture_extension);
	}
	texture_path = texture_file + std::string(".") + texture_extension;

	aiColor4D color;
	from->Get(AI_MATKEY_COLOR_DIFFUSE, color);

	C_Material* material =  new C_Material(nullptr);
	if (texture_path != "" && texture_path != ("."))
	{
		ImportTexture(texture_path.c_str());
		material->texture_id = LoadTexture(texture_file.c_str());
		material->texture_path = texture_path;
	}

	material->color = Color(color.r, color.g, color.b, color.a);

	aiString mat_name;
	from->Get(AI_MATKEY_NAME, mat_name);
	SaveMaterial(material, mat_name.C_Str());
	material = LoadMaterial(mat_name.C_Str());
	return material;
}

void ModuleMaterials::SaveMaterial(const C_Material* material, const char* path)
{
	uint size = sizeof(uint) + material->texture_path.size() + sizeof(float) * 4; //Color size

	// Allocate buffer size
	char* data = new char[size];
	char* cursor = data;

	// Store texture path lenght
	uint bytes = sizeof(uint);
	uint path_size = material->texture_path.size();
	memcpy(cursor, &path_size, bytes);

	cursor += bytes;

	// Store texture path
	bytes = material->texture_path.size();

	if (bytes)
	{
		memcpy(cursor, material->texture_path.c_str(), bytes);
		cursor += bytes;
		
		//memcpy(cursor, "/0", 1);
		//cursor += 1;
	}

	float color[4] { material->color.r, material->color.g, material->color.b, material->color.a };
	bytes = sizeof(float) * 4;
	memcpy(cursor, color, bytes);
	cursor += bytes;

	std::string full_path = ("Library/Materials/");
	full_path.append(path);// .append(".mat");

	App->fileSystem->Save(full_path.c_str(), data, size);

	RELEASE_ARRAY(data);
}

C_Material* ModuleMaterials::LoadMaterial(const char* path)
{
	std::string full_path = "Library/Materials/";
	full_path.append(path);// .append(".mesh");

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;

		C_Material* material = new C_Material(nullptr);

		uint pathSize = 0;
		uint bytes = sizeof(uint);
		//Problem here, check pathSize
		memcpy(&pathSize, cursor, bytes);
		cursor += bytes;

		if (pathSize > 0)
		{
			char* tex_path = new char[pathSize + 1];
			bytes = sizeof(char) * pathSize;
			memcpy(tex_path, cursor, bytes);
			cursor += bytes;
			tex_path[pathSize] = '\0';
			material->texture_path = tex_path;
			RELEASE_ARRAY(tex_path);
		}

		else
		{
			material->texture_path = "";
		}

		if (material->texture_path != "")
		{
			material->texture_id = LoadTexture(material->texture_path.c_str());
		}

		float color[4];
		bytes = sizeof(float) * 4;
		memcpy(color, cursor, bytes);
		cursor += bytes;

		material->color = Color(color[0], color[1], color[2], color[3]);

		material->libFile = path;
		RELEASE_ARRAY(buffer);
		return material;
	}
	else
	{
		LOG("[warning] material file '%s' is empty", path);
		return nullptr;
	}

}

void ModuleMaterials::ImportTexture(const char* path)
{
	std::string ret = "";

	char* buffer = nullptr;
	std::string full_path = "/Textures/";
	full_path.append(path);
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);
	
	if (size > 0)
	{
		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);

		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
		{
			ilEnable(IL_FILE_OVERWRITE);

			ILuint saveBufferSize;
			ILubyte* saveBuffer;

			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			saveBufferSize = ilSaveL(IL_DDS, nullptr, 0);

			if (saveBufferSize > 0)
			{
				saveBuffer = new ILubyte[saveBufferSize];
				if (ilSaveL(IL_DDS, saveBuffer, saveBufferSize) > 0)
				{
					std::string fileName = "";
					App->fileSystem->SplitFilePath(path, nullptr, &fileName);

					std::string save_path = "Library/Textures/";
					ret = save_path.append(fileName);

					App->fileSystem->Save(save_path.c_str(), saveBuffer, saveBufferSize);
					RELEASE_ARRAY(saveBuffer);
				}
			}

			ilDeleteImages(1, &ImageName);
		}
		RELEASE_ARRAY(buffer);
	}
}

uint ModuleMaterials::LoadTexture(const char* path)
{
	uint ret = 0;

	std::string full_path = "Library/Textures/";
	full_path.append(path);// .append(".mesh");


	char* buffer = nullptr;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);

		ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);
		ret = ilutGLBindTexImage();
	}

	return ret;
}