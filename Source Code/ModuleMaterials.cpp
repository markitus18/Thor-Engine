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
	std::string mat_path = "";
	std::string file = "";

	std::string tmpPath = "";
	App->fileSystem->SplitFilePath(path.c_str(), &tmpPath);
	if (numTextures > 0)
	{
		aiString aiStr;
		aiReturn ret = from->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
		App->fileSystem->SplitFilePath(aiStr.C_Str(), &mat_path, &file);
		tmpPath += mat_path + file;
	}
	aiColor4D color;
	from->Get(AI_MATKEY_COLOR_DIFFUSE, color);

	C_Material* material = nullptr;

	if (material != nullptr)
	{
		return material;
	}
	else
	{
		material = new C_Material(nullptr);
		if (mat_path != "" && file != "")
		{
			std::string tex_path = SaveTexture(file.c_str());

			material->texture_path = tex_path;
			material->texture_file = file;
			material->texture_id = LoadTexture(tex_path.c_str());
		}
		else
		{
			material->texture_id = 0;
		}

		material->color = Color(color.r, color.g, color.b, color.a);
		materials.push_back(material);
	}
	
	SaveMaterial(material, file.c_str());
	return material;
}

void ModuleMaterials::SaveMaterial(const C_Material* material, const char* path)
{
	uint size = material->texture_path.size() + sizeof(float) * 4; //Color size

	// Allocate buffer size
	char* data = new char[size];
	char* cursor = data;

	// Store texture path
	uint bytes = material->texture_path.size();;
	memcpy(cursor, material->texture_path.c_str(), bytes);
	cursor += bytes;

	float color[4] { material->color.r, material->color.g, material->color.b, material->color.a };
	bytes = sizeof(float) * 4;
	memcpy(cursor, color, bytes);
	cursor += bytes;

	std::string full_path = ("Library/Materials/");
	full_path.append(path);// .append(".mat");

	App->fileSystem->Save(full_path.c_str(), data, size);

	RELEASE_ARRAY(data);
}

std::string ModuleMaterials::SaveTexture(const char* path)
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
					std::string save_path = "Library/Textures/";
					ret = save_path.append(path).append(".dds");
					App->fileSystem->Save(save_path.c_str(), saveBuffer, saveBufferSize);
					RELEASE_ARRAY(saveBuffer);
				}
			}

			ilDeleteImages(1, &ImageName);
		}
		RELEASE_ARRAY(buffer);
	}
	return ret;
}

uint ModuleMaterials::LoadTexture(const char* path)
{
	uint ret = 0;

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(path, &buffer);

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