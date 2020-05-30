#include "M_Resources.h"
#include "Engine.h"

//Importers
#include "I_Meshes.h"
#include "I_Materials.h"
#include "I_Scenes.h"
#include "I_Animations.h"
#include "I_ParticleSystems.h"
#include "I_Shaders.h"
#include "I_Folders.h"

#include "Resource.h"

#include "M_FileSystem.h"
#include "PathNode.h"

#include "Config.h"
#include "M_Scene.h" //Called only in LoadPrefab method

#include "Assimp/include/scene.h"

M_Resources::M_Resources(bool start_enabled) : Module("Resources", start_enabled)
{

}

M_Resources::~M_Resources()
{

}

bool M_Resources::Init(Config& config)
{
	//ClearMetaData();
	Importer::Textures::Init();

	return true;
}

bool M_Resources::Start()
{
	LoadResourcesLibrary();
	UpdateAssetsImport();
	updateAssets_timer.Start();
	saveChangedResources_timer.Stop();
	return true;
}

update_status M_Resources::Update()
{
	//Little dirty trick to offset both updates
	if (saveChangedResources_timer.IsRunning() == false && updateAssets_timer.ReadSec() > 2.5)
	{
		saveChangedResources_timer.Start();
	}

	if (updateAssets_timer.ReadSec() > 5)
	{
		//UpdateAssetsImport(); //TODO: Add to update in
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
	for (std::map<unsigned long long, Resource*>::iterator it = resources.begin(); it != resources.end(); )
	{
		it->second->FreeMemory();
		RELEASE(it->second);
		it = resources.erase(it);
	}

	return true;
}

void M_Resources::ImportFileFromExplorer(const char* path, const char* dstDir)
{
	std::string normalizedPath = Engine->fileSystem->NormalizePath(path);
	std::string finalPath;
	if (Engine->fileSystem->DuplicateFile(normalizedPath.c_str(), dstDir, finalPath))
	{
		ImportFileFromAssets(finalPath.c_str());
	}
}

uint64 M_Resources::ImportFolderFromAssets(const char* path, uint64 forceID)
{
	Resource* resource = CreateResourceBase(path, Resource::FOLDER, nullptr, forceID);
	const ResourceInfo& resInfo = GetResourceInfo(path, resource->name.c_str(), Resource::FOLDER);
	if (resInfo.ID != 0)
	{
		resource->ID = resInfo.ID;
		resource->instances = DeleteResource(resource->ID);
	}

	resources[resource->ID] = resource;
	resourceLibrary[resource->ID] = GetMetaInfo(resource);
	return resource->ID;
}

uint64 M_Resources::ImportFileFromAssets(const char* path)
{
	Resource::Type type = GetTypeFromFileExtension(path);
	Resource* resource = CreateResourceBase(path, type);
	uint64 resourceID = 0;

	//Checking for an already existing resource. In that case, re-import.
	const ResourceInfo& resInfo = GetResourceInfo(path, resource->name.c_str(), Resource::FOLDER);
	if (resInfo.ID != 0)
	{
		resource->ID = resInfo.ID;
		resource->instances = DeleteResource(resource->ID);
	}	

	char* buffer = nullptr;
	if (uint size = Engine->fileSystem->Load(path, &buffer))
	{
		switch (type)
		{
			case (Resource::TEXTURE):				Importer::Textures::Import(buffer, size, (R_Texture*)resource); break;
			case (Resource::PREFAB):				ImportModel(buffer, size, resource); break;
			case (Resource::SHADER):				Importer::Shaders::Import(buffer, (R_Shader*)resource); break;
			case (Resource::PARTICLESYSTEM):		Importer::Particles::Import(buffer, size, (R_ParticleSystem*)resource); break;
			case (Resource::ANIMATOR_CONTROLLER):	Importer::Animators::Load(buffer, (R_AnimatorController*)resource); break;
		}

		SaveResource(resource);
		resourceID = resource->ID;
		resourceLibrary[resourceID] = GetMetaInfo(resource);
		SaveMetaInfo(resource);

		RELEASE_ARRAY(buffer);
		RELEASE(resource);
	}

	return resourceID;
}

uint64 M_Resources::ImportResourceFromScene(const char* file, const void* data, const char* name, Resource::Type type)
{
	Resource* resource = CreateResourceBase(file, type, name);

	const ResourceInfo& resInfo = GetResourceInfo(file, resource->name.c_str(), Resource::FOLDER);
	if (resInfo.ID != 0)
	{
		resource->ID = resInfo.ID;
		resource->instances = DeleteResource(resource->ID);
	}

	switch (type)
	{
		
		case (Resource::MESH):		Importer::Meshes::Import((aiMesh*)data, (R_Mesh*)resource); break;
		case (Resource::MATERIAL):	Importer::Materials::Import((aiMaterial*)data, (R_Material*)resource); break;
		case (Resource::ANIMATION): Importer::Animations::Import((aiAnimation*)data, (R_Animation*)resource); break;
	}

	SaveResource(resource);

	uint64 resourceID = resource->GetID();
	resourceLibrary[resource->GetID()] = GetMetaInfo(resource);
	RELEASE(resource);

	return resourceID;
}

void M_Resources::ImportModel(const char* buffer, uint size, Resource* prefab)
{
	const aiScene* scene = Importer::Scenes::ProcessAssimpScene(buffer, size);
	Importer::Scenes::Import(scene, (R_Prefab*)prefab);
	std::vector<uint64> meshes, materials, animations;

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		//TODO: building a temporal mesh name. Assimp meshes have no name so they have conflict when importing multiple meshes
		//Unity loads a mesh per each node, and assign the node's name to it
		std::string meshName = prefab->name;
		meshName.append("_mesh").append(std::to_string(i));
		scene->mMeshes[i]->mName = meshName;
		meshes.push_back(ImportResourceFromScene(prefab->original_file.c_str(), scene->mMeshes[i], meshName.c_str(), Resource::Type::MESH));
		prefab->AddContainedResource(meshes.back());
	}
	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		aiString matName;
		scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
		materials.push_back(ImportResourceFromScene(prefab->original_file.c_str(), scene->mMaterials[i], matName.C_Str(), Resource::Type::MATERIAL));
		prefab->AddContainedResource(materials.back());
	}

	for (uint i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		animations.push_back(ImportResourceFromScene(prefab->original_file.c_str(), anim, anim->mName.C_Str(), Resource::Type::ANIMATION));
		prefab->AddContainedResource(animations.back());
	}

	//Link all loaded meshes and materials to the existing gameObjects
	Importer::Scenes::LinkModelResources((R_Prefab*)prefab, meshes, materials);
}

uint64 M_Resources::ImportModelThumbnail(char* buffer, const char* source_file, uint sizeX, uint sizeY)
{
	uint64 ret = 0;
	uint64 newID = 0;
	Resource* resource = nullptr;
	uint64 instances = 0;

	const ResourceInfo& resourceInfo = GetResourceInfo(source_file, "<Thumbnail>", Resource::TEXTURE);

	if (resourceInfo.ID != 0)
	{
		newID = resourceInfo.ID;
		instances = DeleteResource(newID);
	}
	else
	{
		newID = GetNewID();
	}

	//Importing resource
	//resource = Engine->moduleMaterials->ImportModelImage(buffer, newID, source_file, sizeX, sizeY);
	if (resource)
	{
		resource->instances = instances;
		resourceLibrary[resource->ID] = GetMetaInfo(resource);
		SaveMetaInfo(resource);

		RELEASE(resource);
	}
	return newID;
}

Resource* M_Resources::CreateResourceBase(const char* assetsPath, Resource::Type type, const char* name, uint64 forceID)
{
	Resource* resource = nullptr;
	std::string resourcePath;

	static_assert(static_cast<int>(Resource::UNKNOWN) == 10, "Code Needs Update");

	switch (type)
	{
		case Resource::FOLDER:
			resource = (Resource*)Importer::Folders::Create();
			resource->resource_file = FOLDERS_PATH;
			break;
		case Resource::MESH:
			resource = (Resource*)Importer::Meshes::Create();
			resource->resource_file = MESHES_PATH;
			break;
		case Resource::TEXTURE:
			resource = (Resource*)Importer::Textures::Create();
			resource->resource_file = TEXTURES_PATH;
			break;
		case Resource::MATERIAL:
			resource = (Resource*)Importer::Materials::Create();
			resource->resource_file = MATERIALS_PATH;
			break;
		case Resource::ANIMATION:
			resource = (Resource*)Importer::Animations::Create();
			resource->resource_file = ANIMATIONS_PATH;
			break;
		case Resource::ANIMATOR_CONTROLLER:
			resource = (Resource*)Importer::Animators::Create();
			resource->resource_file = ANIMATIONS_PATH;
			break;
		case Resource::PREFAB:
			resource = (Resource*)Importer::Scenes::Create();
			resource->resource_file = GAMEOBJECTS_PATH;
			break;
		case Resource::PARTICLESYSTEM:
			resource = (Resource*)Importer::Particles::Create();
			resource->resource_file = PARTICLES_PATH;
			break;
		case Resource::SHADER:
			resource = (Resource*)Importer::Shaders::Create();
			resource->resource_file = SHADERS_PATH;
			break;
		case Resource::SCENE:
			resource = new Resource(Resource::SCENE);
	}
	
	if (resource == nullptr) return nullptr;

	if (name == nullptr)
	{
		std::string extension;
		Engine->fileSystem->SplitFilePath(assetsPath, nullptr, &resource->name, &extension);
		if (type == Resource::TEXTURE) resource->name.append(".").append(extension);
	}
	else
		resource->name = name;

	resource->ID = forceID ? forceID : GetNewID();
	resource->original_file = assetsPath;
	resource->resource_file.append(std::to_string(resource->ID));

	return resource;
}

uint64 M_Resources::CreateNewCopyResource(const char* srcFile, const char* dstDir)
{
	uint64 resourceID = 0;

	//First we get the resource name and generate a unique one
	std::string path, name, extension;
	Engine->fileSystem->SplitFilePath(srcFile, &path, &name, &extension);
	
	name = Engine->fileSystem->GetUniqueName(dstDir, name.c_str());
	std::string dstPath = std::string(dstDir).append(name);

	//Duplicate the file in assets, import it as usual
	std::string finalPath;
	if (Engine->fileSystem->DuplicateFile(srcFile, dstDir, finalPath))
	{
		resourceID = ImportFileFromAssets(finalPath.c_str());
	}

	return resourceID;
}

Resource* M_Resources::LoadResourceBase(uint64 ID)
{
	Resource* resource = nullptr;
	std::map<uint64, ResourceInfo>::iterator metasIt = resourceLibrary.find(ID);
	if (metasIt != resourceLibrary.end())
	{
 		ResourceInfo& resInfo = metasIt->second;
		resource = CreateResourceBase(resInfo.file.c_str(), resInfo.type, resInfo.name.c_str(), ID);
	}
	return resource;
}

const ResourceInfo& M_Resources::GetResourceInfo(const char* path, const char* name, Resource::Type type) const
{
	//TODO: Search process should start at Root Folder and iterate down the path
	//		It will make it much faster than comparing all strings
	std::map<uint64, ResourceInfo>::const_iterator it;
	
	for (it = resourceLibrary.begin(); it != resourceLibrary.end(); ++it)
	{
		if (it->second.Compare(path, name, type))
			return it->second;
	}

	//TODO: Can this cause errors? Is this a scoped variable that will get removed?
	return ResourceInfo();
}

Resource* M_Resources::GetResource(uint64 ID)
{
	Resource* resource = nullptr;
	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator resourcesIt = resources.find(ID);
	if (resourcesIt != resources.end()) return resourcesIt->second;

	//If the resource is not loaded, search in the library and load it
	if (resource = LoadResourceBase(ID))
	{
		char* buffer = nullptr;
		uint size = Engine->fileSystem->Load(resource->resource_file.c_str(), &buffer);
		if (size == 0)
		{
			RELEASE(resource);
			return nullptr;
		}

		switch (resource->type)
		{
			case (Resource::FOLDER):				{ Importer::Folders::Load(buffer, (R_Folder*)resource); break; }
			case (Resource::MESH):					{ Importer::Meshes::Load(buffer, (R_Mesh*)resource); break; }
			case (Resource::TEXTURE):				{ Importer::Textures::Load(buffer, size, (R_Texture*)resource); break; }
			case (Resource::MATERIAL):				{ Importer::Materials::Load(buffer, size, (R_Material*)resource); break; }
			case (Resource::PREFAB):				{ /*Importer::Scenes::LoadScene();*/ }
			{
				char* metaBuffer = nullptr;
				std::string metaPath = resource->original_file + std::string(".meta");
				if (uint size = Engine->fileSystem->Load(metaPath.c_str(), &metaBuffer))
				{
					Config metaFile(metaBuffer);
					Importer::Scenes::LoadContainedResources(metaFile, (R_Prefab*)resource);
					RELEASE_ARRAY(metaBuffer);
				}
				break;
			}
			case (Resource::ANIMATION):				{ Importer::Animations::Load(buffer, (R_Animation*)resource); break; }
			case (Resource::ANIMATOR_CONTROLLER):	{ Importer::Animators::Load(buffer, (R_AnimatorController*)resource); break; }
			case (Resource::PARTICLESYSTEM):		{ Importer::Particles::Load(buffer, size, (R_ParticleSystem*)resource); break; }
			case (Resource::SHADER):				{ Importer::Shaders::Load(buffer, size, (R_Shader*)resource); break; }
		}
		RELEASE_ARRAY(buffer);

		resources[resource->ID] = resource;
		resource->LoadOnMemory();
	}
	return resource;
}

Resource::Type M_Resources::GetTypeFromFileExtension(const char* path) const
{
	std::string ext = "";
	Engine->fileSystem->SplitFilePath(path, nullptr, nullptr, &ext);

	static_assert(static_cast<int>(Resource::UNKNOWN) == 10, "Code Needs Update");

	if (ext == "FBX" || ext == "fbx")
		return Resource::PREFAB;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "TGA" || ext == "PNG" || ext == "JPG")
		return Resource::TEXTURE;
	if (ext == "shader")
		return Resource::SHADER;
	if (ext == "particles")
		return Resource::PARTICLESYSTEM;
	if (ext == "shader")
		return Resource::SHADER;
	if (ext == "scene")
		return Resource::SCENE;
	if (ext == "anim")
		return Resource::ANIMATION;
	if (ext == "animator")
		return Resource::ANIMATOR_CONTROLLER;
	return Resource::UNKNOWN;
}

bool M_Resources::GetAllMetaFromType(Resource::Type type, std::vector<const ResourceInfo*>& metas) const
{
	std::map<uint64, ResourceInfo>::const_iterator it;
	for (it = resourceLibrary.begin(); it != resourceLibrary.end(); ++it)
	{
		if ((*it).second.type == type)
			metas.push_back(&(*it).second);
	}
	return metas.size() > 0;
}

void M_Resources::LoadModel(uint64 ID)
{
	if (Resource* resource = GetResource(ID))
	{
		char* buffer = nullptr;
		uint size = Engine->fileSystem->Load(resource->resource_file.c_str(), &buffer);
		if (size > 0)
		{
			Config file(buffer);
			Engine->scene->LoadGameObject(file);
		}

		resource->instances++;
	}
}

void M_Resources::LoadResourcesLibrary()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back("meta");

	PathNode engineAssets = Engine->fileSystem->GetAllFiles("Engine/Assets", &filter_ext);
	engineAssets.RemoveFoldersMetas();
	LoadResourceInfoFromFolder(engineAssets);

	PathNode assets = Engine->fileSystem->GetAllFiles("Assets", &filter_ext);
	assets.RemoveFoldersMetas();
	LoadResourceInfoFromFolder(assets);
}

uint64 M_Resources::LoadResourceInfoFromFolder(PathNode node)
{
	uint64 resourceID = 0;
	if (node.isFile)
	{
		resourceID = LoadResourceInfo(node.path.c_str());
	}
	else
	{
		resourceID = LoadFolderInfo(node.path.c_str());
		//If node folder has something inside
		if (!node.isLeaf)
		{
			Resource* folder = GetResource(resourceID);
			folder->containedResources.clear(); //We can clear the contained resources from the folder
												//as they will be added now
			for (uint i = 0; i < node.children.size(); i++)
			{
				if (uint64 containedResourceID = LoadResourceInfoFromFolder(node.children[i]))
					folder->AddContainedResource(containedResourceID);
			}
		}
	}

	return resourceID;
}

uint64 M_Resources::LoadResourceInfo(const char* metaFilePath)
{
	char* buffer = nullptr;

	uint size = Engine->fileSystem->Load(metaFilePath, &buffer);
	uint64 resourceID = 0;

	if (size > 0)
	{
		Config metaFile(buffer);
		RELEASE_ARRAY(buffer);

		std::string fileName = std::string(metaFilePath);
		fileName = fileName.substr(0, fileName.size() - 5);

		ResourceInfo resourceInfo((Resource::Type)(int)metaFile.GetNumber("Type"), fileName.c_str(), metaFile.GetString("Name").c_str(), metaFile.GetNumber("ID"));
		resourceID = resourceInfo.ID;

		//Add all contained resources saved in the meta file
		Config_Array containingResources = metaFile.GetArray("Containing Resources");
		for (uint i = 0; i < containingResources.GetSize(); ++i)
		{
			Config fileNode = containingResources.GetNode(i);
			ResourceInfo containedInfo((Resource::Type)(int)(fileNode.GetNumber("Type")), fileName.c_str(), fileNode.GetString("Name").c_str(), fileNode.GetNumber("ID"));
			resourceLibrary[containedInfo.ID] = containedInfo;
		}
		resourceLibrary[resourceID] = resourceInfo;
	}
	return resourceID;
}

uint64 M_Resources::LoadFolderInfo(const char* file)
{
	uint64 folderID = 0;
	std::string metaPath = std::string(file) + ".meta";

	if (Engine->fileSystem->Exists(metaPath.c_str()))
	{
		char* buffer = nullptr;
		uint size = Engine->fileSystem->Load(metaPath.c_str(), &buffer);
		if (size > 0)
		{
			Config metaFile(buffer);
			RELEASE_ARRAY(buffer);

			folderID = metaFile.GetNumber("ID");
		}
	}

	folderID = ImportFolderFromAssets(file, folderID);
	return folderID;
}

ResourceInfo M_Resources::GetMetaInfo(const Resource* resource) const
{
	return ResourceInfo(resource->GetType(), resource->original_file.c_str(), resource->name.c_str(), resource->GetID());
}

//Save .meta file in assets
void M_Resources::SaveMetaInfo(const Resource* resource)
{
	Config config;

	config.SetNumber("ID", resource->ID);
	config.SetString("Name", resource->name.c_str());
	config.SetNumber("Type", static_cast<int>(resource->GetType()));

	//Getting file modification date
	uint64 modDate = Engine->fileSystem->GetLastModTime(resource->original_file.c_str());
	config.SetNumber("Date", modDate);

	//TODO: Probably a bit dirty to add it here with generic meta files... check later
	if (resource->GetType() == Resource::PREFAB)
	{
		R_Prefab* prefab = (R_Prefab*)resource;
		Importer::Scenes::SaveContainedResources(prefab, config);
	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = resource->original_file + ".meta";
		Engine->fileSystem->Save(path.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
	}
}

void M_Resources::UpdateAssetsImport()
{
	//Getting all files in assets
	std::vector<std::string> ignore_extensions;
	ignore_extensions.push_back("meta");

	PathNode engineAssets = Engine->fileSystem->GetAllFiles("Engine/Assets", nullptr, &ignore_extensions);
	UpdateAssetsFolder(engineAssets);

	PathNode assets = Engine->fileSystem->GetAllFiles("Assets", nullptr, &ignore_extensions);
	UpdateAssetsFolder(assets);
}

uint64 M_Resources::UpdateAssetsFolder(const PathNode& node, bool ignoreResource)
{
	//ResourceID is only returned if the resource is imported as new
	uint64 resourceID = 0;
	if (node.isFile)
	{
		if (!Engine->fileSystem->Exists(std::string(node.path + ".meta").c_str()))
		{
			return ImportFileFromAssets(node.path.c_str());
		}
		else
		{
			if (IsFileModified(node.path.c_str()))
			{
				//TODO: Force ID when re-importing a modified asset
				LOG("File modified: %s", node.path.c_str());
				//uint64 id = GetResourceInfo(node.path.c_str()).id;
				ImportFileFromAssets(node.path.c_str());// , id);
			}
			return 0;
		} 
	}

	//At this point only folders are processed
	Resource* folder = nullptr;
	bool isFolderUpdated = false;

	if (true) //TODO: ignoreResource
	{
		if (!Engine->fileSystem->Exists(std::string(node.path + ".meta").c_str()))
		{
			resourceID = ImportFolderFromAssets(node.path.c_str());
			folder = GetResource(resourceID);
			isFolderUpdated = true;
		}
		else
		{
			uint64 existingID = GetResourceInfo(node.path.c_str()).ID;
			folder = GetResource(existingID);
		}
	}

	for (uint i = 0; i < node.children.size(); i++)
	{
		uint64 childrenID = UpdateAssetsFolder(node.children[i]);
		if (childrenID != 0) //TODO: ignoreResource
		{
			folder->AddContainedResource(childrenID);
			isFolderUpdated = true;
		}
	}

	if (isFolderUpdated)
	{
		SaveResource(folder);
		SaveMetaInfo(folder);
	}

	return resourceID;
}

void M_Resources::ClearMetaData()
{
	//Getting all .meta in assets
	std::vector<std::string> filter_extensions;
	filter_extensions.push_back("meta");

	PathNode engineAssets = Engine->fileSystem->GetAllFiles("Engine", &filter_extensions, nullptr);
	RemoveMetaFromFolder(engineAssets);
	
	PathNode projectFolder = Engine->fileSystem->GetAllFiles("Assets", &filter_extensions, nullptr);
	RemoveMetaFromFolder(projectFolder);

	if (Engine->fileSystem->Exists("Assets.meta"))
		Engine->fileSystem->Remove("Assets.meta");
	Engine->fileSystem->Remove("Library/");
	Engine->fileSystem->CreateLibraryDirectories();
}

void M_Resources::RemoveMetaFromFolder(PathNode node)
{
	if (node.isFile == true)
	{
		Engine->fileSystem->Remove(node.path.c_str());
	}

	//If node folder has something inside
	else if (node.isLeaf == false)
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
	uint size = Engine->fileSystem->Load(meta_file.c_str(), &buffer);

	if (size > 0)
	{
		uint64 fileMod = Engine->fileSystem->GetLastModTime(path);
		uint64 configDate = Config(buffer).GetNumber("Date");
		return fileMod != configDate;
	}
	return false;
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
					char* buffer = nullptr;
					if (uint size = Importer::Materials::Save((R_Material*)it->second, &buffer))
					{
						Engine->fileSystem->Save(it->second->resource_file.c_str(), buffer, size);
						RELEASE_ARRAY(buffer);
					}
				}
			}
			it->second->needs_save = false;
		}
	}
}

uint M_Resources::DeleteResource(uint64 ID)
{
	//TODO: it would be simpler if resource meta stored the path
	//TODO: update folder resource and remove this one from the contained list
	Resource::Type type = resourceLibrary[ID].type;
	uint instances = 0;

	if (resources[ID] != nullptr)
	{
		//Could break here
		instances = resources[ID]->instances;
		UnLoadResource(ID);
		resources.erase(ID);
	}
	
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
	Engine->fileSystem->Remove(resourcePath.c_str());

	resourceLibrary.erase(ID);
	return instances;
}

void M_Resources::UnLoadResource(uint64 ID)
{
	std::map<uint64, Resource*>::iterator it = resources.find(ID);
	if (it != resources.end())
	{
		//TODO: at this moment no resource is freeing any memory
		it->second->FreeMemory();

		Resource* resource = it->second;
		resources.erase(it);
		RELEASE(resource);
	}
}

void M_Resources::SaveResource(Resource* resource)
{
	char* buffer = nullptr;
	uint size = 0;

	switch (resource->GetType())
	{
		case(Resource::FOLDER):					{ size = Importer::Folders::Save((R_Folder*)resource, &buffer); break; }
		case(Resource::MESH):					{ size = Importer::Meshes::Save((R_Mesh*)resource, &buffer); break; }
		case(Resource::TEXTURE):				{ size = Importer::Textures::Save((R_Texture*)resource, &buffer); break; }
		case(Resource::MATERIAL):				{ size = Importer::Materials::Save((R_Material*)resource, &buffer); break; }
		case(Resource::ANIMATION):				{ size = Importer::Animations::Save((R_Animation*)resource, &buffer); break;	}
		case(Resource::ANIMATOR_CONTROLLER):	{ size = Importer::Animators::Save((R_AnimatorController*)resource, &buffer);	break; }
		case(Resource::PREFAB):					{ size = Importer::Scenes::SaveScene((R_Prefab*)resource, &buffer); break; }
		case(Resource::PARTICLESYSTEM):			{ size = Importer::Particles::Save((R_ParticleSystem*)resource, &buffer); break; }
		case(Resource::SHADER):					{ size = Importer::Shaders::Save((R_Shader*)resource, &buffer); break; }
		case(Resource::SCENE):					{ /*size = Importer::Scenes::SaveScene((R_Prefab*)resource, &buffer);*/ break; }
	}

	if (size > 0)
	{
		Engine->fileSystem->Save(resource->resource_file.c_str(), buffer, size);

		if (IsModifiableResource(resource))
		{
			Engine->fileSystem->Save(resource->original_file.c_str(), buffer, size);
		}
		RELEASE_ARRAY(buffer);
	}
}

bool M_Resources::IsModifiableResource(const Resource* resource) const
{
	return resource->isInternal == false;
}