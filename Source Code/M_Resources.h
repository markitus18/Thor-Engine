#ifndef __M_RESOURCES_H__
#define __M_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "Timer.h"
#include "MathGeoLib\src\Algorithm\Random\LCG.h"

#include <map>
#include <vector>

class R_Prefab;
struct PathNode;

struct ResourceInfo
{
	Resource::Type type = Resource::UNKNOWN;
	std::string file = "";
	std::string name = "";
	uint64 ID = 0;

	ResourceInfo() {}; //Looks like we need default constructor for maps
	ResourceInfo(Resource::Type type, const char* file, const char* name, uint64 id) : type(type), file(file), name(name), ID(id) {};

	bool Compare(const char* file, const char* name, Resource::Type type) const
	{
		return (this->file == file && (name ? this->name == name : true) && (type != Resource::Type::UNKNOWN ? type == this->type : true));
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

	//Import a file from outside the project folder
	//The file will be duplicated in the current active folder in the asset explorer
	void ImportFileFromExplorer(const char* path, const char* dstDir);

	//Import a file existing in assets and create its resources
	uint64 ImportFileFromAssets(const char* path);

	//Import a folder existing in assets and create it as a resource
	uint64 ImportFolderFromAssets(const char* path);
	
	//Import a 3D scene file
	void ImportModel(const char* buffer, uint size, Resource* model);

	//Import a resource existing in a prefab (3D scene) file
	uint64 ImportResourceFromScene(const char* file, const void* data, const char* name, Resource::Type type);

	uint64 ImportModelThumbnail(char* buffer, const char* source_file, uint sizeX, uint sizeY);

	//Generates the base data for a resource
	Resource* CreateResourceBase(const char* assetsPath, Resource::Type type, const char* name = nullptr, uint64 forceID = 0);

	//Used for internal resources (external referring to fbx, textures,...)
	uint64 CreateNewCopyResource(const char* srcFile, const char* dstDir);
	
	//Finds the ID in the resource library and creates the resource base of that type
	Resource* LoadResourceBase(uint64 ID);

	const ResourceInfo& GetResourceInfo(const char* path, const char* name = nullptr, Resource::Type type = Resource::UNKNOWN) const;

	Resource* GetResource(uint64 ID);

	Resource::Type GetTypeFromFileExtension(const char* path) const;
	bool GetAllMetaFromType(Resource::Type type, std::vector<const ResourceInfo*>& metas) const;

	void LoadModel(uint64 ID);

	inline uint64 GetNewID() { return random.Int(); }

private:
	void LoadResourceLibrary();
	void LoadLibraryFromFolder(PathNode node);
	void LoadResourceInfo(const char* file);

	ResourceInfo	GetMetaInfo(const Resource* resource) const;

	//.meta file generation
	void SaveMetaInfo(const Resource* resource);

	//Search through all assets and imports / re-imports
	void UpdateAssetsImport();
	uint64 UpdateAssetsFolder(const PathNode& node, bool ignoreResource = false);

	//Remove all .meta files in assets
	void ClearMetaData();

	//Remove all .meta files in a folder
	void RemoveMetaFromFolder(PathNode node);

	bool IsFileModified(const char* path);

	void SaveChangedResources();

	//Completely deletes a resource, including its file (not yet though)
	//Return number of instances previous to deletion
	uint DeleteResource(uint64 ID);
	//Removes a resource from memory
	void UnLoadResource(uint64 ID);

	void SaveResource(Resource* resource);

	//Returns if this resource can be modified in Assets file and thus has to be overitten there too
	bool IsModifiableResource(const Resource* resource) const;

public:
	//Resources loaded in memory
	//TODO: Move to private, accessing in W_Resources to display memory
	std::map<uint64, Resource*> resources;

private:

	//All resources imported
	std::map<uint64, ResourceInfo> resourceLibrary;

	Timer updateAssets_timer;
	Timer saveChangedResources_timer;
	LCG random;
};

#endif //!__M_RESOURCES_H__
