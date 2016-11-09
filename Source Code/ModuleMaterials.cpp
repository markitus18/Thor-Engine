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
			material->texture_path = mat_path + file;
			material->texture_file = file;
			material->texture_id = ImportTexture(tmpPath.c_str());
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

uint ModuleMaterials::ImportTexture(const char* file)
{
	//TODO: Search for all "Textures" folder and search that file ?
	uint ret = 0;

	char* buffer = nullptr;
	std::string full_path = file;//"/Textures/";
	//full_path.append(file);

	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

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
					save_path.append(file).append(".dds");
					App->fileSystem->Save(save_path.c_str(), data, bufSize);
					RELEASE_ARRAY(data);
				}
			}
		}

		RELEASE_ARRAY(buffer);
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