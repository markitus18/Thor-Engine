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

	//C_Material* ImportMaterial(const aiMaterial* from, const std::string& texture_path);

	//void SaveMaterial(const C_Material* material, const char* path);
	//C_Material* LoadMaterial(const char* path);

	////Both import and save
	//void ImportTexture(const char* path);
	//uint LoadTexture(const char* path);

	
	R_Material* ImportMaterialResource(const aiMaterial* mat, unsigned long long ID, const char* source_file);	
	bool SaveMaterialResource(const R_Material* mat);
	R_Material* LoadMaterialResource(unsigned long long ID);

	//Both import and save
	R_Texture* ImportTextureResource(const char* buffer, unsigned long long ID, const char* file, uint size);
	R_Texture* LoadTextureResource(unsigned long long ID);
};

#endif // __MODULE_MATERIALS_H__
