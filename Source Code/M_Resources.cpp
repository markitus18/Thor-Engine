#include "M_Resources.h"
#include "Application.h"

//Loaders
#include "M_Meshes.h"
#include "M_Materials.h"
#include "M_Import.h"

//Resources
#include "R_Mesh.h"
#include "R_Texture.h"
#include "R_Material.h"
#include "R_Prefab.h"

#include "M_FileSystem.h"
#include "Config.h"
#include "M_Scene.h"

#include <time.h>

M_Resources::M_Resources(bool start_enabled) : Module("Resources", start_enabled)
{

}

M_Resources::~M_Resources()
{

}

bool M_Resources::Init(Config& config)
{
	//ClearMetaData();
	return true;
}

bool M_Resources::Start()
{
	LoadResourcesData();
	UpdateAssetsImport();
	updateAssets_timer.Start();
	saveChangedResources_timer.Stop();
	return true;
}

update_status M_Resources::Update(float dt)
{
	//Little dirty trick to offset both updates
	if (saveChangedResources_timer.IsRunning() == false && updateAssets_timer.ReadSec() > 2.5)
	{
		saveChangedResources_timer.Start();
	}

	if (updateAssets_timer.ReadSec() > 5)
	{
		UpdateAssetsImport();
		updateAssets_timer.Start();
	}

	if (saveChangedResources_timer.ReadSec() > 5)
	{
		SaveChangedResources();
		saveChangedResources_timer.Start();
	}

	return UPDATE_CONTINUE;
}

bool M_Resources::CleanUp()
{
	SaveChangedResources();
	SaveResourcesData();
	for (std::map<unsigned long long, Resource*>::iterator it = resources.begin(); it != resources.end(); )
	{
		it->second->FreeMemory();
		RELEASE(it->second);
		it = resources.erase(it);
	}

	return true;
}

void M_Resources::ImportFileFromAssets(const char* path, uint64 ID)
{
	Resource::Type type = GetTypeFromPath(path);
	switch (type)
	{
		case (Resource::TEXTURE):
		{
			char* buffer = nullptr;
			uint size = App->fileSystem->Load(path, &buffer);
			ImportRTexture(buffer, path, size, ID);
			break;
		}
		case (Resource::PREFAB):
		{
			ImportScene(path, false);
			break;
		}
	}
}

void M_Resources::ImportScene(const char* source_file, bool add)
{
	R_Prefab* resource = nullptr;

	std::string name = "";
	App->fileSystem->SplitFilePath(source_file, nullptr, &name);
	resource = (R_Prefab*)FindResourceInLibrary(source_file, name.c_str(), Resource::PREFAB);
	if (resource != nullptr)
	{
		if (add)
			resource->instances++;
		return;// resource;
	}


	resource = App->moduleImport->ImportFile(source_file, ++nextID);
	if (resource)
	{
		AddResource(resource);
		if (add)
			resource->instances++;
	}
	//return resource;
}

uint64 M_Resources::ImportRMesh(const aiMesh* mesh, const char* source_file, const char* name, uint64 ID)
{
	uint64 ret = 0;
	R_Mesh* resource = nullptr;

	//If ID is not 0 it means we have a resource imported, but we want to reimport it
	if (ID == 0)
	{
		//First we find if that resource has already been imported
		resource = (R_Mesh*)FindResourceInLibrary(source_file, name, Resource::MESH);
		if (resource != nullptr)
			return resource->ID;
	}
	else
	{
		DeleteResource(ID);
	}
	//If its not imported, import it
	uint64 newID = ID == 0 ? ++nextID : ID;
	resource = App->moduleMeshes->ImportMeshResource(mesh, newID, source_file, name);
	if (resource)
	{
		AddResource(resource);
		ret = resource->ID;
	}

	return ret;
}

uint64 M_Resources::ImportRTexture(const char* buffer, const char* source_file, uint size, uint64 ID)
{
	uint64 ret = 0;
	R_Texture* resource = nullptr;

	//If ID is not 0 it means we have a resource imported and want to reimport it
	if (ID == 0)
	{
		//First we find if that resource has already been imported
		resource = (R_Texture*)FindResourceInLibrary(source_file, "", Resource::TEXTURE);
		if (resource != nullptr)
			return resource->ID;;
	}
	
	else //erase the old resource
	{
		DeleteResource(ID);
	}

	//Importing resource
	uint64 newID = ID == 0 ? ++nextID : ID;
	resource = App->moduleMaterials->ImportTextureResource(buffer, newID, source_file, size);
	if (resource)
	{
		AddResource(resource);
		ret = resource->ID;
	}
	return ret;
}

uint64 M_Resources::ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name, uint64 ID)
{
	uint64 ret = 0;
	R_Material* resource = nullptr;
	
	//First we find if that resource has already been imported
	resource = (R_Material*)FindResourceInLibrary(source_file, name, Resource::MATERIAL);
	if (resource != nullptr)
	{
		return resource->ID;
	}

	//If its not imported, import it
	resource = App->moduleMaterials->ImportMaterialResource(mat, ++nextID, source_file);
	if (resource)
	{
		AddResource(resource);
		ret = resource->ID;
	}
	return ret;
}

Resource* M_Resources::GetResource(uint64 ID, Resource::Type type)
{
	Resource* ret = nullptr;
	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator it = resources.find(ID);
	if (it != resources.end())
	{
		ret = it->second;
	}
	else if (type != Resource::UNKNOWN)
	{	//If resource is not loaded, search in library
		std::map<uint64, ResourceMeta>::iterator it = existingResources.find(ID);
		if (it != existingResources.end())
		{
			switch (type)
			{
				case (Resource::MESH):
				{
					ret = App->moduleMeshes->LoadMeshResource(ID);
					LoadResource(ret);
					break;
				}
				case (Resource::TEXTURE):
				{
					ret = App->moduleMaterials->LoadTextureResource(ID);
					ret->original_file = it->second.original_file;
					ret->name = it->second.resource_name;
					LoadResource(ret);
					break;
				}
				case (Resource::MATERIAL):
				{
					ret = App->moduleMaterials->LoadMaterialResource(ID);
					if (((R_Material*)ret)->textureID != 0)
					{
						R_Texture* rTex = (R_Texture*)GetResource(((R_Material*)ret)->textureID, Resource::TEXTURE);
						rTex->instances++;
					}
					LoadResource(ret);
					break;
				}
				case (Resource::PREFAB):
				{
					//Anything to do when Getting a prefab resource?
					//AddResource(ret);
					break;
				}
				case (Resource::BONE):
				{
					break;
				}
			}

			if (ret != nullptr)
			{
				ret->original_file = it->second.original_file;
				ret->name = it->second.resource_name;
				LOG("Resource load from library correctly");
			}
		}
		else
		{
			LOG("Could not find resource ID %lld", ID);
		}
	
	}
	return ret;
}

Resource::Type M_Resources::GetTypeFromPath(const char* path)
{
	std::string ext = "";
	App->fileSystem->SplitFilePath(path, nullptr, nullptr, &ext);

	if (ext == "FBX" || ext == "fbx")
		return Resource::PREFAB;
	if (ext == "tga" || ext == "png" || ext == "jpg")
		return Resource::TEXTURE;
	return Resource::UNKNOWN;
}

void M_Resources::LoadPrefab(const char* path)
{
	//By now we will asume everytime we want to load an fbx into the scene, it has already been imported
	for (std::map<uint64, Resource*>::const_iterator it = resources.begin(); it != resources.end(); it++)
	{
		if (it->second->original_file == path && it->second->type == Resource::PREFAB)
		{
			App->scene->LoadGameObject(it->first);
			it->second->instances++;
			return;
		}
	}
	for (std::map<uint64, ResourceMeta>::const_iterator it = existingResources.begin(); it != existingResources.end(); it++)
	{
		if (it->second.original_file == path && it->second.type == Resource::PREFAB)
		{
			App->scene->LoadGameObject(it->first);
			R_Prefab* resource = new R_Prefab;
			resource->ID = it->first;
			resource->original_file = it->second.original_file;
			resource->name = it->second.resource_name;
			resource->resource_file = std::string("/Library/GameObjects/");
			resource->resource_file.append(std::to_string(it->first));
			resource->instances++;
			AddResource(resource);
			return;
		}
	}

	LOG("Could not find file '%s' loaded in resources", path);
}

PathNode M_Resources::CollectImportedScenes()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back("FBX");
	filter_ext.push_back("fbx");

	return App->fileSystem->GetAllFiles("Assets", &filter_ext);
}

void M_Resources::SaveResourcesData()
{
	Config config;
	Config_Array array = config.SetArray("Resources");

	for (std::map<uint64, ResourceMeta>::iterator it = existingResources.begin(); it != existingResources.end(); it++)
	{
		Config node = array.AddNode();
		node.SetNumber("ID", it->first);
		node.SetString("OriginalFile", it->second.original_file.c_str());
		node.SetString("ResourceName", it->second.resource_name.c_str());
		node.SetNumber("Type", static_cast<int>(it->second.type));
	}

	config.SetNumber("NextID", nextID);

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	App->fileSystem->Save(metaFile.c_str(), buffer, size);
}

void M_Resources::LoadResourcesData()
{
	bool json_file = false;
	if (json_file)
	{
#pragma region JSON_File
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(metaFile.c_str(), &buffer);
		if (size > 0 && buffer != nullptr)
		{
			Config config(buffer);
			Config_Array array = config.GetArray("Resources");

			for (uint i = 0; i < array.GetSize(); i++)
			{
				Config node = array.GetNode(i);
				ResourceMeta meta;
				meta.original_file = node.GetString("OriginalFile");
				meta.resource_name = node.GetString("ResourceName");
				meta.type = static_cast<Resource::Type>((int)node.GetNumber("Type"));
				existingResources[node.GetNumber("ID")] = meta;
			}

			nextID = config.GetNumber("NextID");
		}
#pragma endregion
	}
	else
	{
#pragma region Meta_Files
		std::vector<std::string> filter_ext;
		filter_ext.push_back("meta");

		//TODO: to be able to do this it is necessary to create ".material" files, otherwise
		//there is no .meta file for the material and thus the material resource is not found
		PathNode assets = App->fileSystem->GetAllFiles("Assets", &filter_ext);
		LoadMetaFromFolder(assets);

#pragma endregion
	}
}

void M_Resources::LoadMetaFromFolder(PathNode node)
{
	if (node.file == true)
	{
		LoadMetaInfo(node.path.c_str());
	}

	//If node folder has something inside
	else if (node.leaf == false)
	{
		for (uint i = 0; i < node.children.size(); i++)
		{
			LoadMetaFromFolder(node.children[i]);
		}
	}

}

Resource* M_Resources::FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type)
{
	for (std::map<uint64, ResourceMeta>::iterator it = existingResources.begin(); it != existingResources.end(); it++)
	{
		if (it->second.Compare(original_file, name))
		{
			return GetResource(it->first, type);
		}
	}
	return nullptr;
}

ResourceMeta M_Resources::GetMetaInfo(Resource* resource)
{
	ResourceMeta ret;

	ret.original_file = resource->original_file;
	ret.resource_name = resource->name;
	ret.type = resource->type;

	return ret;
}

bool M_Resources::LoadMetaInfo(const char* file)
{
	char* buffer = nullptr;
	uint size = App->fileSystem->Load(file, &buffer);
	ResourceMeta meta;
	if (size > 0)
	{
		Config config(buffer);

		meta.original_file = file;
		meta.resource_name = config.GetString("ResourceName");
		meta.id = config.GetNumber("ResourceID");
		meta.type = static_cast<Resource::Type>((int)(config.GetNumber("Type")));
		existingResources[meta.id] = meta;

		if (meta.type == Resource::PREFAB)
		{
			std::string resFile = "/Library/GameObjects/";
			resFile.append(std::to_string(meta.id));
			LoadSceneMeta(resFile.c_str());
		}

		return true;
	}
	return false;
}

bool M_Resources::LoadSceneMeta(const char* file)
{
	char* buffer = nullptr;
	uint size = App->fileSystem->Load(file, &buffer);
	if (size > 0)
	{
		Config config(buffer);
		Config_Array GameObjects = config.GetArray("GameObjects");

		for (uint i = 0; i < GameObjects.GetSize(); i++)
		{
			Config node = GameObjects.GetNode(i);
			ResourceMeta meta;
	
			if (node.GetNumber("MeshID"))
			{
				//Loop through 
				meta.original_file = file;
				meta.id = node.GetNumber("MeshID");
				meta.type = Resource::MESH;
				existingResources[meta.id] = meta;
			}

			if (node.GetNumber("MaterialID"))
			{
				meta.original_file = file;
				meta.id = node.GetNumber("MaterialID");
				meta.type = Resource::MATERIAL;
				existingResources[meta.id] = meta;
			}
		}
		return true;
	}
	return false;
}

//Save .meta file in assets
void M_Resources::SaveMetaInfo(const Resource* resource)
{
	Config config;

	config.SetNode("Metadata");
	config.SetNumber("ResourceID", resource->ID);
	config.SetString("ResourceName", resource->name.c_str());
	config.SetNumber("Type", static_cast<int>(resource->GetType()));

	//Getting file modification date
	std::string global_path = "";
	App->fileSystem->GetRealDir(resource->original_file.c_str(), global_path);

	uint64 modDate = App->fileSystem->GetLastModTime(resource->original_file.c_str());
	config.SetNumber("ModificationDate", modDate);
	
	if (resource->type == Resource::PREFAB)
	{

	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = resource->original_file + ".meta";
		App->fileSystem->Save(path.c_str(), buffer, size);
	}
}

void M_Resources::SaveFileDate(const char* path, Config& config)
{
	struct tm* clock;
	struct stat attr;

	stat(path, &attr);
	clock = localtime(&(attr.st_mtime));
	
	config.SetNumber("Day", clock->tm_mday);
	config.SetNumber("Month", clock->tm_mon);
	config.SetNumber("Year", clock->tm_year);
	config.SetNumber("Hour", clock->tm_hour);
	config.SetNumber("Minutes", clock->tm_min);
	config.SetNumber("Seconds", clock->tm_sec);
}

void M_Resources::UpdateAssetsImport()
{
	//Getting all files in assets
	std::vector<std::string> ignore_extensions;
	ignore_extensions.push_back("meta");
	PathNode assets = App->fileSystem->GetAllFiles("Assets", nullptr, &ignore_extensions);
	UpdateAssetsFolder(assets);
}

void M_Resources::UpdateAssetsFolder(const PathNode& node)
{
	//If node is a file
	if (node.file == true)
	{
		if (App->fileSystem->Exists(std::string(node.path + ".meta").c_str()) == false)
		{
			ImportFileFromAssets(node.path.c_str());
		}
		else
		{
			if (IsFileModified(node.path.c_str()))
			{
				LOG("File modified: %s", node.path.c_str());
				std::string meta_file = node.path + (".meta");
				uint64 id = GetIDFromMeta(meta_file.c_str());
				ImportFileFromAssets(node.path.c_str(), id);
			}
		}
	}
	//If node folder has something inside
	else if (node.leaf == false)
	{
		for (uint i = 0; i < node.children.size(); i++)
		{
			UpdateAssetsFolder(node.children[i]);
		}
	}
}

void M_Resources::ClearMetaData()
{
	//Getting all .meta in assets
	std::vector<std::string> filter_extensions;
	filter_extensions.push_back("meta");
	PathNode assets = App->fileSystem->GetAllFiles("Assets", &filter_extensions, nullptr);
	RemoveMetaFromFolder(assets);
}

void M_Resources::RemoveMetaFromFolder(PathNode node)
{
	if (node.file == true)
	{
		App->fileSystem->Remove(node.path.c_str());
	}

	//If node folder has something inside
	else if (node.leaf == false)
	{
		for (uint i = 0; i < node.children.size(); i++)
		{
			RemoveMetaFromFolder(node.children[i]);
		}
	}
}

bool M_Resources::IsFileModified(const char* path)
{
	std::string meta_file = path;
	meta_file.append(".meta");

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(meta_file.c_str(), &buffer);

	if (size > 0)
	{
		uint64 fileMod = App->fileSystem->GetLastModTime(path);
		uint64 configData = Config(buffer).GetNumber("ModificationDate");
		return fileMod != configData;
	}
	return false;
}

uint64 M_Resources::GetIDFromMeta(const char* path)
{
	uint64 ret = 0;

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(path, &buffer);
	
	if (size > 0)
		ret = Config(buffer).GetNumber("ResourceID");

	return ret;
}

void M_Resources::SaveChangedResources()
{
	for (std::map<uint64, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
	{
		if (it->second->needs_save == true)
		{
			switch (it->second->type)
			{
				//By now the only resource that can be modified is material
				case(Resource::MATERIAL):
				{
					App->moduleMaterials->SaveMaterialResource((R_Material*)it->second);
				}
			}
			it->second->needs_save = false;
		}
	}
}

void M_Resources::AddResource(Resource* resource)
{
	LoadResource(resource);
	existingResources[resource->ID] = GetMetaInfo(resource);
	SaveMetaInfo(resource);
}

void M_Resources::LoadResource(Resource* resource)
{
	resources[resource->ID] = resource;
	switch (resource->type)
	{
	case(Resource::MESH):
	{
		meshes[resource->ID] = resource;
		break;
	}
	case(Resource::MATERIAL):
	{
		materials[resource->ID] = resource;
		break;
	}
	case (Resource::TEXTURE):
	{
		textures[resource->ID] = resource;
		break;
	}
	case(Resource::PREFAB):
	{
		scenes[resource->ID] = resource;
		break;
	}
	}
	resource->LoadOnMemory();
}

void M_Resources::DeleteResource(uint64 ID)
{
	Resource::Type type = resources[ID]->type;
	UnLoadResource(ID);

	std::string resourcePath = "";
	switch (type)
	{
		case(Resource::MESH):
		{
			resourcePath.append("/Library/Meshes/");
			break;
		}
		case(Resource::MATERIAL):
		{
			resourcePath.append("/Library/Materials/");
			break;
		}
		case(Resource::TEXTURE):
		{
			resourcePath.append("/Library/Textures/");
			break;
		}
		case(Resource::PREFAB):
		{
			resourcePath.append("/Library/GameObjects/");
			break;
		}
	}
	resourcePath.append(std::to_string(ID));
	App->fileSystem->Remove(resourcePath.c_str());
}

void M_Resources::UnLoadResource(uint64 ID)
{
	std::map<uint64, Resource*>::iterator it = resources.find(ID);
	if (it != resources.end())
	{
		it->second->FreeMemory();

		switch (it->second->type)
		{
			case(Resource::MESH):
			{
				meshes.erase(ID);
				break;
			}
			case(Resource::MATERIAL):
			{
				materials.erase(ID);
				break;
			}
			case(Resource::TEXTURE):
			{
				textures.erase(ID);
				break;
			}
			case(Resource::PREFAB):
			{
				scenes.erase(ID);
				break;
			}
		}
		Resource* resource = it->second;
		resources.erase(it);
		RELEASE(resource);
	}
}