#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "ResourceHandle.h"

#include "Timer.h"
#include "MathGeoLib\src\Algorithm\Random\LCG.h"

#include <map>
#include <vector>

class R_Folder;
struct PathNode;

class M_Resources : public Module
{
public:
	M_Resources(bool start_enabled = true);
	~M_Resources();

	bool Init(Config& config) override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	//Import a file from outside the project folder
	//The file will be duplicated into the current active folder in the asset explorer
	void ImportFileFromExplorer(const char* path, const char* dstDir);

	//Import a file existing in assets and create its resources
	uint64 ImportFileFromAssets(const char* path);
	
	Resource* RequestResource(uint64 ID);
	void ReleaseResource(Resource* resource);

	//Used only for internal resources (external referring to DCC tools)
	uint64 CreateNewCopyResource(const char* srcFile, const char* dstDir);

	//Calls importers to generate a filled binary file and saves it into Library
	//If the resource can be modified in Assets, it is saved there as well
	void SaveResource(Resource* resource, bool saveMeta = true);

	//Saves the current resource as a new resource located at 'newPath'
	//Returns the newly created resource's ID
	uint64 SaveResourceAs(Resource* resource, const char* directory, const char* fileName);

	const ResourceBase* FindResourceBase(const char* path, const char* name = nullptr, ResourceType type = ResourceType::UNKNOWN) const;
	const ResourceBase* GetResourceBase(uint64 ID) const;

	bool GetAllMetaFromType(ResourceType type, std::vector<const ResourceBase*>& metas) const;

private:
	//Iterates all files inside Assets and Engine/Assets.
	//Loads any resource that is not registered inside resourcesLibrary
	//Imports any new resource
	void LoadAllAssets();

	//Loads the base data from the resource in 'node.path' and all its children
	//Returns whether the resource was imported as new or not
	bool LoadAssetBase(PathNode node, uint64& assetID);

	//Import a 3D scene file
	void ImportModel(const char* buffer, uint size, Resource* prefab);

	//Import a resource existing in a prefab (3D scene) file
	uint64 ImportResourceFromModel(const char* file, const void* data, const char* name, ResourceType type);

	//Generates the model's thumbnail and saves it in library
	uint64 ImportModelThumbnail(char* buffer, const char* source_file, uint sizeX, uint sizeY);

	//Generates the base data for a resource
	Resource* CreateNewResource(const char* assetsPath, ResourceType type, const char* name = nullptr);

	//Creates a resource from the base data in the library
	Resource* CreateResourceFromBase(ResourceBase& base);

	//.meta file generation
	void SaveMetaInfo(ResourceBase& base);

	void SaveChangedResources();

	//Completely deletes a resource
	//'deleteAsset' flag to delete the original asset file
	//Return number of instances previous to deletion
	uint DeleteResource(uint64 ID, bool deleteAsset = true);

	//Deletes the memory from a loaded resource
	//Returns the instances held by the resource
	uint UnloadResource(uint64 ID);

	//Remove all .meta files in assets
	void ClearMetaData();

	//Remove all .meta files in a folder
	void RemoveMetaFromFolder(PathNode node);

	ResourceType GetTypeFromFileExtension(const char* path) const;
	inline uint64 GetNewID() { return random.Int(); }

public:
	//Resources loaded in memory
	//TODO: Move to private, accessing in W_Resources to display memory
	std::map<uint64, Resource*> resources;

	ResourceHandle<Resource> hAssetsFolder;
	ResourceHandle<Resource> hEngineAssetsFolder;
private:

	//All resources imported
	std::map<uint64, ResourceBase> resourceLibrary;
	
	Timer updateAssets_timer;
	Timer saveChangedResources_timer;
	LCG random;
};

#endif //!__M_RESOURCES_H__
