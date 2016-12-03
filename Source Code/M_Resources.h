#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "Timer.h"

#include <map>
#include <vector>

struct aiMesh;
struct aiMaterial;

class R_Mesh;
class R_Texture;
class R_Material;
struct PathNode;

struct ResourceMeta
{
	Resource::Type type;
	std::string original_file;
	std::string resource_name;

	bool Compare(const char* file, const char* name)
	{
		return (original_file == file && resource_name == name);
	}

};

class M_Resources : public Module
{
public:
	M_Resources(bool start_enabled = true);
	~M_Resources();

	bool Init(Config& config);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//Import a file existing in assets creating the resources
	void ImportFileFromAssets(const char* path);

	void ImportScene(const char* source_file);
	R_Mesh* ImportRMesh(const aiMesh* from, const char* source_file, const char* name);
	R_Texture* ImportRTexture(const char* buffer, const char* path, uint size);
	R_Material* ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name);

	///Getting a resource by ID
	//Resource PREFAB creates a new GameObject in the scene
	Resource* GetResource(uint64 ID, Resource::Type type);
	Resource::Type GetTypeFromPath(const char* path);

	void LoadPrefab(const char* path);

	//Release all imported resources data
	void FinishImporting();

private:
	void SaveResourcesData();
	void LoadResourcesData();

	Resource*		FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type);
	ResourceMeta	GetMetaInfo(Resource* resource);

	//Meta data management -----------------------------------------------------
	void SaveMetaInfo(const Resource* resource);
	void SaveFileDate(const char* path, Config& config);

	void UpdateAssetsImport();
	void UpdateAssetsFolder(const PathNode& node);

	//Remove all .meta files in assets TODO: fix fileSystem removing error
	void ClearMetaData();

	//Remove all .meta files in a folder
	void RemoveMetaFromFolder(PathNode node);

	bool IsFileModified(const char* path);
	//---------------------------------------------------------------------------
private:
	//Resources loaded in memory
	std::map<uint64, Resource*> resources;

	//Resources used for an importing file. Deleted after
	//importion is completed
	std::vector<Resource*> importingResources;

	//All resources imported
	std::map<uint64, ResourceMeta> existingResources;

	std::string metaFile = "/ProjectSettings/Resources.JSON";

	uint64 nextID = 0;

	Timer updateAssets;
};

#endif
