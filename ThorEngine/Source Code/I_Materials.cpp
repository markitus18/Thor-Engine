#include "I_Materials.h"

#include "Engine.h"
#include "M_FileSystem.h"

#include "R_Texture.h"
#include "R_Material.h"
#include "M_Resources.h"

#include "Assimp/include/material.h"
#include "Assimp/include/texture.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#include "Devil\include\ilu.h"
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#include "Devil\include\ilut.h"
#pragma comment( lib, "Devil/libx86/ILUT.lib" )


/*
R_Texture* M_Materials::ImportPrefabImage(char* buffer, uint64 ID, const char* source_file, uint sizeX, uint sizeY)
{
	R_Texture* resTexture = nullptr;

	std::string full_path("/Library/Textures/");
	full_path.append(std::to_string(ID));

	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ilTexImage(sizeX, sizeY, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, buffer);

	//Saving file
	ILuint saveBufferSize;
	ILubyte* saveBuffer;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

	ilEnable(IL_FILE_OVERWRITE);
	ilSave(IL_DDS, full_path.c_str());
	
	saveBufferSize = ilSaveL(IL_DDS, nullptr, 0);

	if (saveBufferSize > 0)
	{
		saveBuffer = new ILubyte[saveBufferSize];
		if (ilSaveL(IL_DDS, saveBuffer, saveBufferSize) > 0)
		{
			Engine->fileSystem->Save(full_path.c_str(), saveBuffer, saveBufferSize);
			RELEASE_ARRAY(saveBuffer);

			resTexture = new R_Texture;
			resTexture->original_file = full_path;
			resTexture->resource_file = full_path;
			resTexture->ID = ID;
			resTexture->buffer = ilutGLBindTexImage();
			std::string file, extension;
			Engine->fileSystem->SplitFilePath(resTexture->original_file.c_str(), nullptr, &file, &extension);
			resTexture->name = file + (".") + extension;
		}
	}

	ilDeleteImages(1, &img);

	return resTexture;
}
*/

R_Material* Importer::Materials::Create()
{
	return new R_Material();
}

//TODO: Find texture in hard drive, duplicate it and import it next to the model
void Importer::Materials::Import(const aiMaterial* material, R_Material* rMaterial)
{
	uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
	std::string texture_fileName = "", texture_extension = "";
	std::string texture_file;
	std::string texture_path;

	if (numTextures > 0)
	{
		aiString aiStr;
		aiReturn ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr);
		Engine->fileSystem->SplitFilePath(aiStr.C_Str(), nullptr, &texture_fileName, &texture_extension);
	}

	//TODO: Dirty pathing done here
	texture_file = texture_fileName + std::string(".") + texture_extension;
	texture_path = "/Assets/Textures/";
	texture_path.append(texture_file);

	aiColor4D color;
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

	if (texture_file != "" && texture_file != ("."))
	{
		if (uint64 textureID = Engine->moduleResources->ImportFileFromAssets(texture_path.c_str()))
		{
			rMaterial->hTexture.Set(textureID);
		}
	}

	rMaterial->color = Color(color.r, color.g, color.b, color.a);

	aiString matName;
	material->Get(AI_MATKEY_NAME, matName);
	rMaterial->baseData->name = matName.C_Str();
}

//Process R_Material data into a buffer ready to save
//Returns the size of the buffer file (0 if any errors)
//Warning: buffer memory needs to be released after the function call
uint64 Importer::Materials::Save(const R_Material* rMaterial, char** buffer)
{
	//Name size, name string, texture resource ID, color
	uint size = sizeof(unsigned long long) + sizeof(float) * 4;

	*buffer = new char[size];
	char* cursor = *buffer;

	uint64 textureID = rMaterial->hTexture.GetID();
	uint bytes = sizeof(unsigned long long);
	memcpy(cursor, &textureID, bytes);
	cursor += bytes;

	float color[4]{ rMaterial->color.r, rMaterial->color.g, rMaterial->color.b, rMaterial->color.a };
	bytes = sizeof(float) * 4;
	memcpy(cursor, color, bytes);
	cursor += bytes;

	return size;
}

//Process buffer data into a ready-to-use R_Material.
//Returns nullptr if any errors occured during the process.
void Importer::Materials::Load(const char* buffer, uint size, R_Material* rMaterial)
{
	const char* cursor = buffer;

	uint64 textureID = 0;
	uint bytes = sizeof(unsigned long long);
	memcpy(&textureID, cursor, bytes);
	cursor += bytes;
	rMaterial->hTexture.Set(textureID);

	float color[4];
	bytes = sizeof(float) * 4;
	memcpy(color, cursor, bytes);
	cursor += bytes;

	rMaterial->color = Color(color[0], color[1], color[2], color[3]);
}

void Importer::Textures::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

R_Texture* Importer::Textures::Create()
{
	return new R_Texture();
}

bool Importer::Textures::Import(const char* buffer, uint size, R_Texture* rTexture)
{
	//Warning: cannot check if buffer is nullptr: could begin with /0
	if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
	{
		return true;
	}
	return false;
}

uint64 Importer::Textures::Save(const R_Texture* rTexture, char** buffer)
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
			*buffer = (char*)saveBuffer;
		}
	}
	return saveBufferSize;
}

void Importer::Textures::Load(const char* buffer, uint size, R_Texture* texture)
{
	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);
	texture->buffer = ilutGLBindTexImage();

	ilDeleteImages(1, &ImageName);
}
