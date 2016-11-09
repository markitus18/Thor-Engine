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

C_Material* ModuleMaterials::ImportMaterial(const aiMaterial* from, const char* global_scene_path, const char* local_scene_path)
{
	uint numTextures = from->GetTextureCount(aiTextureType_DIFFUSE);

	std::string texture_global_path = "";
	std::string scene_dir = "";
	App->fileSystem->SplitFilePath(global_scene_path, &scene_dir);

	if (numTextures > 0)
	{
		aiString aiStr;
		aiReturn ret = from->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
		texture_global_path = global_scene_path;
		texture_global_path.append(aiStr.C_Str());
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
		if (texture_global_path != "")
		{
			std::string file;
			App->fileSystem->SplitFilePath(texture_global_path.c_str(), nullptr, &file);
			std::string texture_local_path = local_scene_path + std::string("/Textures/") + file;
			material->texture_path = texture_local_path;
			material->texture_file = file;
			material->texture_id = ImportTexture(texture_global_path.c_str(), texture_local_path.c_str());
		}
		else
		{
			material->texture_id = 0;
		}

		material->color = Color(color.r, color.g, color.b, color.a);
		materials.push_back(material);
	}
	
	return material;
}

uint ModuleMaterials::ImportTexture(const char* global_path, const char* local_path)
{
	uint ret = 0;
	//First of all, duplicate texture in assets
	if (App->fileSystem->CopyNewFile(global_path, local_path))
	{
		//LEFT HERE
		char* buffer = nullptr;

		uint size = App->fileSystem->Load(local_path, &buffer);

		if (size > 0)
		{
			ILuint ImageName;
			ilGenImages(1, &ImageName);
			ilBindImage(ImageName);

 			if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
			{
				ilEnable(IL_FILE_OVERWRITE);

				//Loading texture buffer
				ret = ilutGLBindTexImage();
				ilDeleteImages(1, &ImageName);

				//Saving texture in dds
				ILuint bufSize;
				ILubyte* data;

				ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
				bufSize = ilSaveL(IL_DDS, nullptr, 0);

				if (bufSize > 0)
				{
					data = new ILubyte[bufSize];
					if (ilSaveL(IL_DDS, data, bufSize) > 0)
					{
						std::string save_path = "Library/Textures/";
						std::string fileName;
						App->fileSystem->SplitFilePath(local_path, nullptr, &fileName);
						save_path.append(fileName).append(".dds");
						App->fileSystem->Save(save_path.c_str(), data, bufSize);
						RELEASE_ARRAY(data);
					}
				}
			}

			RELEASE_ARRAY(buffer);
		}
	}

	return ret;
}

//Tmp function, move to file system
void ModuleMaterials::CutPath(std::string& str)
{
	uint position = str.find_last_of("\\/");
	if (position != std::string::npos)
	{
		str = str.substr(position + 1, str.size() - position);
	}
}

std::string ModuleMaterials::GetFileFolder(const std::string& str)
{
	std::string ret;
	uint position = str.find_last_of("\\/");
	if (position != std::string::npos)
	{
		ret = str.substr(0, position + 1);
	}
	return ret;
}