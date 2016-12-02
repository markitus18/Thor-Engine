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

#include <Windows.h>

M_Resources::M_Resources(bool start_enabled) : Module("Resources", start_enabled)
{

}

M_Resources::~M_Resources()
{

}

bool M_Resources::Init(Config& config)
{
	//char* buffer = nullptr;
	//uint size = App->fileSystem->Load("/Assets/Textures/building03_c.tga", &buffer);
	LoadResourcesData();
	//UpdateAssetsImport();
	return true;
}

bool M_Resources::Start()
{
	//ClearMetaData();
	UpdateAssetsImport();
	return true;
}

bool M_Resources::CleanUp()
{
	SaveResourcesData();
	for (std::map<unsigned long long, Resource*>::iterator it = resources.begin(); it != resources.end(); )
	{
		it->second->FreeMemory();
		RELEASE(it->second);
		it = resources.erase(it);
	}

	return true;
}

void M_Resources::ImportFileFromAssets(const char* path)
{
	Resource::Type type = GetTypeFromPath(path);
	switch (type)
	{
		case (Resource::TEXTURE):
		{
			char* buffer = nullptr;
			uint size = App->fileSystem->Load(path, &buffer);
			ImportRTexture(buffer, path, size);
			break;
		}
		case (Resource::PREFAB):
		{
			ImportScene(path);
			break;
		}
	}
}

void M_Resources::ImportScene(const char* source_file)
{
	R_Prefab* resource = nullptr;

	resource = (R_Prefab*)FindResourceInLibrary(source_file, "", Resource::PREFAB);
	if (resource != nullptr)
		return;// resource;

	resource = App->moduleImport->ImportFile(source_file, ++nextID);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		//TODO: simply release resource?
		importingResources.push_back(resource);
		SaveMetaInfo(resource);
	}
	//return resource;
}

R_Mesh* M_Resources::ImportRMesh(const aiMesh* mesh, const char* source_file, const char* name)
{
	R_Mesh* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Mesh*)FindResourceInLibrary(source_file, name, Resource::MESH);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMeshes->ImportMeshResource(mesh, ++nextID, source_file, name);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
	}

	return resource;
}

R_Texture* M_Resources::ImportRTexture(const char* buffer, const char* source_file, uint size)
{
	R_Texture* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Texture*)FindResourceInLibrary(source_file, "", Resource::TEXTURE);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMaterials->ImportTextureResource(buffer, ++nextID, source_file, size);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
		SaveMetaInfo(resource);
	}
	return resource;
}

R_Material* M_Resources::ImportRMaterial(const aiMaterial* mat, const char* source_file, const char* name)
{
	R_Material* resource = nullptr;

	//First we find if that resource has already been imported
	resource = (R_Material*)FindResourceInLibrary(source_file, name, Resource::MATERIAL);
	if (resource != nullptr)
		return resource;

	//If its not imported, import it
	resource = App->moduleMaterials->ImportMaterialResource(mat, ++nextID, source_file);
	if (resource)
	{
		existingResources[resource->ID] = GetMetaInfo(resource);
		importingResources.push_back(resource);
	}
	return resource;
}

Resource* M_Resources::GetResource(uint64 ID, Resource::Type type)
{
	Resource* ret = nullptr;
	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator it = resources.find(ID);
	if (it != resources.end())
	{
		ret = it->second;
		LOG("Getting resource already loaded");
	}
	else
	{
	
		//If resource is not loaded, search in library
		switch (type)
		{
			case (Resource::MESH):
			{
				ret = App->moduleMeshes->LoadMeshResource(ID);
				break;
			}
			case (Resource::TEXTURE):
			{
				ret = App->moduleMaterials->LoadTextureResource(ID);
				break;
			}
			case (Resource::MATERIAL):
			{
				ret = App->moduleMaterials->LoadMaterialResource(ID);
			}
			case (Resource::BONE):
			{
				break;
			}
		}

		if (ret != nullptr)
		{
			ret->LoadOnMemory();
			LOG("Resource load from library correctly");
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

void M_Resources::FinishImporting()
{
	for (std::vector<Resource*>::iterator it = importingResources.begin(); it != importingResources.end(); /**/)
	{
		RELEASE(*it);
		it = importingResources.erase(it);
	}
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
	}

	config.SetNumber("NextID", nextID);

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	App->fileSystem->Save(metaFile.c_str(), buffer, size);
}

void M_Resources::LoadResourcesData()
{
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

			existingResources[node.GetNumber("ID")] = meta;
		}

		nextID = config.GetNumber("NextID");
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

void M_Resources::SaveMetaInfo(const Resource* resource)
{
	Config config;

	config.SetNode("Metadata");
	config.SetNumber("ResourceID", resource->ID);
	
	//Getting file modification date
	std::string global_path = "";
	App->fileSystem->GetRealDir(resource->original_file.c_str(), global_path);
	HANDLE hFile;
	hFile = CreateFile(global_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME creationTime, lpLastAccesTime, lastWriteTime;

		bool error = GetFileTime(hFile, &creationTime, &lpLastAccesTime, &lastWriteTime);
		if (error != false)
		{
			SYSTEMTIME systemTime;
			bool res = FileTimeToSystemTime(&lastWriteTime, &systemTime);
			if (res != false)
			{
				config.SetNumber("Day", systemTime.wDay);
				config.SetNumber("Hour", systemTime.wHour);
				config.SetNumber("Minutes", systemTime.wMinute);
				config.SetNumber("Seconds", systemTime.wSecond);
			}
		}
		//Warning: It is important to close the handle. Filesystem won't be able to access it otherwise
		CloseHandle(hFile);
	}
	else
	{
		LOG("Could not fild file date");
	}
	
	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = resource->original_file + ".meta";
		App->fileSystem->Save(path.c_str(), buffer, size);
	}
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