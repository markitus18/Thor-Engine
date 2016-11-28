#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include <map>
#include <vector>

struct aiMesh;
struct aiMaterial;

class R_Mesh;
class R_Texture;
class R_Material;

struct ResourceMeta
{
	Resource::Type type;
	std::string original_file;
	std::string resource_name;

	bool Compare(const char* file, const char* name)
	{
		if (original_file == file)
		{
			if (std::string(name) != "")
			{
				if (resource_name == name)
					return true;
			}
			else
				return true;
		}
	}
};

class M_Resources : public Module
{
public:
	M_Resources(bool start_enabled = true);
	~M_Resources();

	bool Init(Config& config);
	bool CleanUp();

	R_Mesh* ImportRMesh(const aiMesh* from, const char* source_file, const char* name);
	R_Texture* ImportRTexture(const char* buffer, const char* path, uint size);
	R_Material* ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name);

	Resource* GetResource(uint64 ID, Resource::Type type);

	void FinishImporting();

private:
	Resource*		FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type);
	ResourceMeta	GetMetaInfo(Resource* resource);

private:
	//Resources loaded in memory
	std::map<uint64, Resource*> resources;

	//Resources used for an importing file. Deleted after
	//importion is completed
	std::vector<Resource*> importingResources;

	//All resources imported
	std::map<uint64, ResourceMeta> existingResources;

	unsigned long long nextID = 0;
};

#endif
