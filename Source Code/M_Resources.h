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
	Resource::Type type = Resource::UNKNOWN;
	std::string original_file = "";
	std::string resource_name = "";
	uint64 id = 0;

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
	void ImportFileFromAssets(const char* path, uint64 ID = 0);

	void ImportScene(const char* source_file, bool add = true);
	uint64 ImportRMesh(const aiMesh* from, const char* source_file, const char* name, uint64 ID = 0);
	uint64 ImportRTexture(const char* buffer, const char* path, uint size, uint64 ID = 0);
	uint64 ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name, uint64 ID = 0);

	///Getting a resource by ID
	//Resource PREFAB creates a new GameObject in the scene
	Resource* GetResource(uint64 ID, Resource::Type type = Resource::UNKNOWN);
	Resource::Type GetTypeFromPath(const char* path);

	void LoadPrefab(const char* path);

	PathNode CollectImportedScenes();


private:
	void SaveResourcesData();
	void LoadResourcesData();
	void LoadMetaFromFolder(PathNode node);

	Resource*		FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type);
	ResourceMeta	GetMetaInfo(Resource* resource);
	bool			LoadMetaInfo(const char* file);
	bool			LoadSceneMeta(const char* file, const char* source_file);

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
	uint64 GetIDFromMeta(const char* path);
	//---------------------------------------------------------------------------

	void SaveChangedResources();

	//Adds a new resource (importion previous to this)
	void AddResource(Resource* resource);
	//Loads an existing resource. Loading is previous to this, this is just for data management
	void LoadResource(Resource* resource);
	//Completely deletes a resource, including its file (not yet though)
	void DeleteResource(uint64 ID);
	//Removes a resource from memory
	void UnLoadResource(uint64 ID);

public:
	//Just for quick info display
	std::map<uint64, Resource*> meshes;
	std::map<uint64, Resource*> materials;
	std::map<uint64, Resource*> textures;
	std::map<uint64, Resource*> scenes;

private:
	//Resources loaded in memory
	std::map<uint64, Resource*> resources;

	//All resources imported
	std::map<uint64, ResourceMeta> existingResources;

	std::string metaFile = "/ProjectSettings/Resources.JSON";

	uint64 nextID = 1;

	Timer updateAssets_timer;
	Timer saveChangedResources_timer;
};

#endif
