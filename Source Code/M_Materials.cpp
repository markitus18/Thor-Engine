#include "M_Materials.h"

#include "Application.h"
#include "M_FileSystem.h"

#include "C_Material.h"
#include "Color.h"
#include "R_Texture.h"
#include "R_Material.h"
#include "M_Resources.h"

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



M_Materials::M_Materials(bool start_enabled) : Module("Materials", start_enabled)
{

}

M_Materials::~M_Materials()
{

}

bool M_Materials::Init()
{
	return true;
}

bool M_Materials::CleanUp()
{
	return true;
}

R_Material* M_Materials::ImportMaterialResource(const aiMaterial* mat, unsigned long long ID, const char* source_file)
{
	uint numTextures = mat->GetTextureCount(aiTextureType_DIFFUSE);
	std::string texture_fileName = "", texture_extension = "";
	std::string texture_file;
	std::string texture_path;

	R_Material* material = new R_Material;

	if (numTextures > 0)
	{
		aiString aiStr;
		aiReturn ret = mat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
		App->fileSystem->SplitFilePath(aiStr.C_Str(), nullptr, &texture_fileName, &texture_extension);
	}

	texture_file = texture_fileName + std::string(".") + texture_extension;
	texture_path = "/Assets/Textures/";
	texture_path.append(texture_file);

	aiColor4D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

	if (texture_file != "" && texture_file != ("."))
	{
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(texture_path.c_str(), &buffer);

		if (size > 0)
		{
			R_Texture* rTex = App->moduleResources->ImportRTexture(buffer, texture_path.c_str(), size);
			if (rTex != nullptr)
				material->textureID = rTex->ID;
		}
	}

	material->color = Color(color.r, color.g, color.b, color.a);
	material->ID = ID;

	aiString matName;
	mat->Get(AI_MATKEY_NAME, matName);

	material->name = matName.C_Str();
	material->original_file = source_file;

	SaveMaterialResource(material);

	return material;
}

bool M_Materials::SaveMaterialResource(const R_Material* mat)
{
	//Name size, name string, texture resource ID, color
	uint size = sizeof(uint) + mat->name.size() + sizeof(unsigned long long) + sizeof(float) * 4;

	char* data = new char[size];
	char* cursor = data;

	// Store mat name lenght
	uint bytes = sizeof(uint);
	uint path_size = mat->name.size();
	memcpy(cursor, &path_size, bytes);
	cursor += bytes;

	// Store mat name
	bytes = mat->name.size();

	if (bytes)
	{
		memcpy(cursor, mat->name.c_str(), bytes);
		cursor += bytes;
	}

	bytes = sizeof(unsigned long long);
	memcpy(cursor, &mat->textureID, bytes);
	cursor += bytes;

	float color[4]{ mat->color.r, mat->color.g, mat->color.b, mat->color.a };
	bytes = sizeof(float) * 4;
	memcpy(cursor, color, bytes);
	cursor += bytes;

	std::string full_path = ("/Library/Materials/");
	full_path.append(std::to_string(mat->ID));

	uint ret = App->fileSystem->Save(full_path.c_str(), data, size);

	RELEASE_ARRAY(data);

	return ret > 0;
}

R_Material* M_Materials::LoadMaterialResource(unsigned long long ID)
{
	//TODO: MATERIAL SAVE FILE ORIGIN
	std::string full_path = "/Library/Materials/";
	full_path.append(std::to_string(ID));

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	R_Material* material = nullptr;

	if (size > 0)
	{
		char* cursor = buffer;

		material = new R_Material;

		uint nameSize = 0;
		uint bytes = sizeof(uint);
		memcpy(&nameSize, cursor, bytes);
		cursor += bytes;

		if (nameSize > 0)
		{
			char* name = new char[nameSize + 1];
			bytes = sizeof(char) * nameSize;
			memcpy(name, cursor, bytes);
			cursor += bytes;

			name[nameSize] = '\0';
			material->name = name;
			RELEASE_ARRAY(name);
		}

		bytes = sizeof(unsigned long long);
		memcpy(&material->textureID, cursor, bytes);
		cursor += bytes;

		float color[4];
		bytes = sizeof(float) * 4;
		memcpy(color, cursor, bytes);
		cursor += bytes;

		material->color = Color(color[0], color[1], color[2], color[3]);

		material->texture = (R_Texture*)App->moduleResources->GetResource(material->textureID, Resource::TEXTURE);
		material->resource_file = full_path;
		material->ID = ID;

		RELEASE_ARRAY(buffer);
	}
	return material;
}

R_Texture* M_Materials::ImportTextureResource(const char* buffer, unsigned long long ID, const char* file, uint size)
{
	R_Texture* resTexture = nullptr;

	std::string full_path("/Library/Textures/");
	full_path.append(std::to_string(ID));

	//Warning: cannot check if buffer is nullptr: could begin with /0
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
				App->fileSystem->Save(full_path.c_str(), saveBuffer, saveBufferSize);
				RELEASE_ARRAY(saveBuffer);

				resTexture = new R_Texture;
				resTexture->original_file = file;
				resTexture->resource_file = full_path;
				resTexture->ID = ID;
			}
		}
	}

	return resTexture;
}

R_Texture* M_Materials::LoadTextureResource(unsigned long long ID)
{
	std::string full_path = "/Library/Textures/";
	full_path.append(std::to_string(ID));

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	R_Texture* rTexture = nullptr;
	if (size > 0)
	{
		rTexture = new R_Texture;

		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);

		ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);
		rTexture->buffer = ilutGLBindTexImage();

		ilDeleteImages(1, &ImageName);
		RELEASE_ARRAY(buffer);
	}

	return rTexture;
}
