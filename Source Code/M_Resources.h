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

	Resource* TestFunction() { LOG("BEEEEP"); }

	bool Init(Config& config) override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	//Import a file from outside the project folder
	//The file will be duplicated into the current active folder in the asset explorer
	void ImportFileFromExplorer(const char* path, const char* dstDir);

	//Import a file existing in assets and create its resources
	uint64 ImportFileFromAssets(const char* path);
	
	//Import a 3D scene file
	void ImportModel(const char* buffer, uint size, Resource* prefab);

	//Import a resource existing in a prefab (3D scene) file
	uint64 ImportResourceFromScene(const char* file, const void* data, const char* name, ResourceType type);

	//Generates the model's thumbnail and saves it in library
	uint64 ImportModelThumbnail(char* buffer, const char* source_file, uint sizeX, uint sizeY);

	//Generates the base data for a resource
	Resource* CreateNewResource(const char* assetsPath, ResourceType type, const char* name = nullptr, uint64 forceID = 0);

	//Creates a resource from the base data in the library
	Resource* CreateResourceFromBase(ResourceBase& base);

	//Used for internal resources (external referring to fbx, textures,...)
	uint64 CreateNewCopyResource(const char* srcFile, const char* dstDir);
	
	//Finds the ID in the resource library and creates the resource base of that type
	Resource* LoadResourceBase(uint64 ID);

	//Calls importers to generate a filled binary file and saves it into Library
	//If the resource can be modified in Assets, it is saved there aswell
	void SaveResource(Resource* resource);

	//Saves the current resource as a new resource located at 'newPath'
	//Returns the newly created resource's ID
	uint64 SaveResourceAs(Resource* resource, const char* directory, const char* fileName);

	const ResourceBase& GetResourceBase(const char* path, const char* name = nullptr, ResourceType type = ResourceType::UNKNOWN) const; //TODO: Change name for FindResourceBase
	const ResourceBase& GetResourceBase(uint64 ID) const;

	Resource* RequestResource(uint64 ID);
	void ReleaseResource(Resource* resource);

	ResourceType GetTypeFromFileExtension(const char* path) const;
	bool GetAllMetaFromType(ResourceType type, std::vector<const ResourceBase*>& metas) const;

	inline uint64 GetNewID() { return random.Int(); }

private:
	void LoadResourcesLibrary();
	uint64 LoadResourceInfoFromFolder(PathNode node);
	uint64 LoadResourceInfo(const char* file);

	//.meta file generation
	void SaveMetaInfo(const ResourceBase& base);

	//Search through all assets. Detects if there is a new/modified file and (re)imports
	void UpdateAssetsImport();

	uint64 UpdateAssetsFolder(const PathNode& node, bool ignoreResource = false);

	//Remove all .meta files in assets
	void ClearMetaData();

	//Remove all .meta files in a folder
	void RemoveMetaFromFolder(PathNode node);

	bool IsFileModified(const char* path);

	void SaveChangedResources();

	//Completely deletes a resource
	//'deleteAsset' flag to delete the original asset file
	//Return number of instances previous to deletion
	uint DeleteResource(uint64 ID, bool deleteAsset = true);

	//Deletes the memory from a loaded resource
	void UnLoadResource(uint64 ID);

	//Returns if this resource can be modified in Assets file and thus has to be overitten there too
	bool IsModifiableResource(const Resource* resource) const;

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
