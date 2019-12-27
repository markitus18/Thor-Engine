#ifndef __MODULE_MATERIALS_H__
#define __MODULE_MATERIALS_H__

#include "Module.h"
#include "Color.h"
#include <list>

class C_Material;
struct aiMaterial;
class R_Texture;
class R_Material;

class M_Materials : public Module
{
public:
	M_Materials(bool start_enabled = true);
	~M_Materials();

	bool Init();
	bool CleanUp();
	
	R_Material* ImportMaterialResource(const aiMaterial* mat, unsigned long long ID, const char* source_file);	
	bool SaveMaterialResource(const R_Material* mat);
	R_Material* LoadMaterialResource(unsigned long long ID);

	//Both import and save
	R_Texture* ImportTextureResource(const char* buffer, unsigned long long ID, const char* file, uint size);
	R_Texture* ImportPrefabImage(char* buffer, uint64 ID, const char* source_file, uint sizeX, uint sizeY);

	R_Texture* LoadTextureResource(unsigned long long ID);
};

namespace Importer
{
	namespace Materials
	{
		//Processes aiMaterial data into a ready-to-use R_Material to be saved later.
		//Returns nullptr if any errors occured during the process.
		void Import(const aiMaterial* material, R_Material* rMaterial);

		//Process R_Material data into a buffer ready to save
		//Returns the size of the buffer file (0 if any errors)
		//Warning: buffer memory needs to be released after the function call
		uint64 Save(const R_Material* rMaterial, char** buffer);

		//Process buffer data into a ready-to-use R_Material.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_Material* rMaterial);
	}

	namespace Textures
	{
		//Processes buffer data into a ready-to-use R_Texture to be saved later.
		//Returns nullptr if any errors occured during the process.
		bool Import(const char* buffer, uint size, R_Texture* rTexture);

		//This function is just to keep consistency in all importers. Save NEEDS to be called after
		//Import due to DevIL memory management
		uint64 Save(const R_Texture* rTexture, char** buffer);

		//Process buffer data into a ready-to-use R_Mesh.
		//Returns nullptr if any errors occured during the process.
		void Load(const char* buffer, uint size, R_Texture* texture);
	}
}

#endif // __MODULE_MATERIALS_H__
