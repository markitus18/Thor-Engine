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
#include "ResourceHandle.h"

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
	ClearMetaData();
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

uint64 M_Resources::ImportFileFromAssets(const char* path)
{
	ResourceType type = GetTypeFromFileExtension(path);
	Resource* resource = CreateNewResource(path, type);
	uint64 resourceID = resource->GetID();

	char* buffer = nullptr;
	if (uint size = Engine->fileSystem->Load(path, &buffer))
	{
		switch (type)
		{
			case (ResourceType::TEXTURE):				Importer::Textures::Import(buffer, size, (R_Texture*)resource); break;
			case (ResourceType::MODEL):					ImportModel(buffer, size, resource); break;
			case (ResourceType::SHADER):				Importer::Shaders::Import(buffer, (R_Shader*)resource); break;
			case (ResourceType::PARTICLESYSTEM):		Importer::Particles::Import(buffer, size, (R_ParticleSystem*)resource); break;
			case (ResourceType::ANIMATION):				Importer::Animations::Load(buffer, (R_Animation*)resource); break;
			case (ResourceType::ANIMATOR_CONTROLLER):	Importer::Animators::Load(buffer, (R_AnimatorController*)resource); break;
			case (ResourceType::SCENE):					Importer::Scenes::Load(buffer, (R_Scene*)resource);
		}

		SaveResource(resource);
		SaveMetaInfo(*resource->baseData);

		RELEASE_ARRAY(buffer);
		UnLoadResource(resource->GetID());
	}

	return resourceID;
}

uint64 M_Resources::ImportResourceFromScene(const char* file, const void* data, const char* name, ResourceType type)
{
	Resource* resource = CreateNewResource(file, type, name);
	uint64 resourceID = resource->GetID();

	switch (type)
	{	
		case (ResourceType::MESH):		Importer::Meshes::Import((aiMesh*)data, (R_Mesh*)resource); break;
		case (ResourceType::MATERIAL):	Importer::Materials::Import((aiMaterial*)data, (R_Material*)resource); break;
		case (ResourceType::ANIMATION): Importer::Animations::Import((aiAnimation*)data, (R_Animation*)resource); break;
	}

	SaveResource(resource);
	UnLoadResource(resource->GetID());

	return resourceID;
}

void M_Resources::ImportModel(const char* buffer, uint size, Resource* model)
{
	const aiScene* scene = Importer::Models::ProcessAssimpScene(buffer, size);
	Importer::Models::Import(scene, (R_Model*)model);
	std::vector<uint64> meshes, materials, animations;

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		//TODO: building a temporal mesh name. Assimp meshes have no name so they have conflict when importing multiple meshes
		//Unity loads a mesh per each node, and assign the node's name to it
		std::string meshName = model->GetName();
		meshName.append("_mesh").append(std::to_string(i));
		scene->mMeshes[i]->mName = meshName;
		meshes.push_back(ImportResourceFromScene(model->GetAssetsFile(), scene->mMeshes[i], meshName.c_str(), ResourceType::MESH));
		model->AddContainedResource(meshes.back());
	}
	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		aiString matName;
		scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
		materials.push_back(ImportResourceFromScene(model->GetAssetsFile(), scene->mMaterials[i], matName.C_Str(), ResourceType::MATERIAL));
		model->AddContainedResource(materials.back());
	}

	for (uint i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		animations.push_back(ImportResourceFromScene(model->GetAssetsFile(), anim, anim->mName.C_Str(), ResourceType::ANIMATION));
		model->AddContainedResource(animations.back());
	}

	//Link all loaded meshes and materials to the existing gameObjects
	Importer::Models::LinkModelResources((R_Model*)model, meshes, materials);
}

uint64 M_Resources::ImportModelThumbnail(char* buffer, const char* source_file, uint sizeX, uint sizeY)
{
	uint64 ret = 0;
	uint64 newID = 0;
	Resource* resource = nullptr;
	uint64 instances = 0;

	//resource = Engine->moduleMaterials->ImportModelImage(buffer, newID, source_file, sizeX, sizeY);
	if (resource)
	{
		SaveMetaInfo(*resource->baseData);
		RELEASE(resource);
	}
	return newID;
}

Resource* M_Resources::CreateNewResource(const char* assetsPath, ResourceType type, const char* name, uint64 forceID)
{
	uint64 newID = forceID ? forceID : GetNewID();
	uint oldInstances = DeleteResource(newID, false); //DeleteResource already checks if it exists

	ResourceBase& base = resourceLibrary[newID] = ResourceBase(type, assetsPath, name, newID);
	if (name == nullptr)
	{
		std::string extension;
		Engine->fileSystem->SplitFilePath(assetsPath, nullptr, &base.name, &extension);
		if (type == ResourceType::TEXTURE) base.name.append(".").append(extension);
	}

	static_assert(static_cast<int>(ResourceType::UNKNOWN) == 10, "Code Needs Update");
	switch (type)
	{
		case ResourceType::FOLDER:				base.libraryFile = FOLDERS_PATH; break;
		case ResourceType::MESH:				base.libraryFile = MESHES_PATH;	break;
		case ResourceType::TEXTURE:				base.libraryFile = TEXTURES_PATH; break;
		case ResourceType::MATERIAL:			base.libraryFile = MATERIALS_PATH; break;
		case ResourceType::ANIMATION:			base.libraryFile = ANIMATIONS_PATH;	break;
		case ResourceType::ANIMATOR_CONTROLLER:	base.libraryFile = ANIMATIONS_PATH;	break;
		case ResourceType::MODEL:				base.libraryFile = MODELS_PATH;	break;
		case ResourceType::PARTICLESYSTEM:		base.libraryFile = PARTICLES_PATH; break;
		case ResourceType::SHADER:				base.libraryFile = SHADERS_PATH; break;
		case ResourceType::SCENE:				base.libraryFile = SCENES_PATH;	break;
	}
	base.libraryFile.append(std::to_string(newID));

	Resource* newResource = CreateResourceFromBase(base);
	newResource->instances = oldInstances;

	return 	newResource;
}

Resource* M_Resources::CreateResourceFromBase(ResourceBase& base)
{
	Resource* resource = nullptr;
	static_assert(static_cast<int>(ResourceType::UNKNOWN) == 10, "Code Needs Update");

	switch (base.type)
	{
		case ResourceType::FOLDER:				resource = (Resource*)Importer::Folders::Create(); break;
		case ResourceType::MESH:				resource = (Resource*)Importer::Meshes::Create(); break;
		case ResourceType::TEXTURE:				resource = (Resource*)Importer::Textures::Create(); break;
		case ResourceType::MATERIAL:			resource = (Resource*)Importer::Materials::Create(); break;
		case ResourceType::ANIMATION:			resource = (Resource*)Importer::Animations::Create(); break;
		case ResourceType::ANIMATOR_CONTROLLER:	resource = (Resource*)Importer::Animators::Create(); break;
		case ResourceType::MODEL:				resource = (Resource*)Importer::Models::Create(); break;
		case ResourceType::PARTICLESYSTEM:		resource = (Resource*)Importer::Particles::Create(); break;
		case ResourceType::SHADER:				resource = (Resource*)Importer::Shaders::Create(); break;
		case ResourceType::SCENE:				resource = (Resource*)Importer::Scenes::Create(); break;
	}

	resource->baseData = &base;
	return resources[base.ID] = resource;
}

uint64 M_Resources::CreateNewCopyResource(const char* srcFile, const char* dstDir)
{
	uint64 resourceID = 0;

	//First we get the resource name and generate a unique one
	std::string path, name, extension;
	Engine->fileSystem->SplitFilePath(srcFile, &path, &name, &extension);
	
	name = Engine->fileSystem->GetUniqueName(dstDir, name.c_str());
	std::string dstPath = std::string(dstDir).append(name).append(".").append(extension);

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
	std::map<uint64, ResourceBase>::iterator metasIt = resourceLibrary.find(ID);
	if (metasIt != resourceLibrary.end())
	{
		ResourceBase& base = metasIt->second;
		resource = CreateResourceFromBase(base);
	}
	return resource;
}

const ResourceBase& M_Resources::GetResourceBase(const char* path, const char* name, ResourceType type) const
{
	//TODO: Search process should start at Root Folder and iterate down the path
	//		It will make it much faster than comparing all strings
	std::map<uint64, ResourceBase>::const_iterator it;
	
	for (it = resourceLibrary.begin(); it != resourceLibrary.end(); ++it)
	{
		if (it->second.Compare(path, name, type))
			return it->second;
	}

	//TODO: Can this cause errors? Is this a scoped variable that will get removed?
	return ResourceBase();
}

const ResourceBase& M_Resources::GetResourceBase(uint64 ID) const
{
	std::map<uint64, ResourceBase>::const_iterator it = resourceLibrary.find(ID);
	if (it != resourceLibrary.end())
	{
		return it->second;
	}
	return ResourceBase();
}

Resource* M_Resources::RequestResource(uint64 ID)
{
	Resource* resource = nullptr;

	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator resourcesIt = resources.find(ID);
	if (resourcesIt != resources.end())
	{
		resourcesIt->second->instances++;
		return resourcesIt->second;
	}

	//If the resource is not loaded, search in the library and load it
	if (resource = LoadResourceBase(ID))
	{
		char* buffer = nullptr;
		uint size = Engine->fileSystem->Load(resource->GetLibraryFile(), &buffer);
		if (size == 0)
		{
			//TODO: A resource does not have a valid library file, needs re-import
			RELEASE(resource);
			return nullptr;
		}

		switch (resource->GetType())
		{
			case (ResourceType::FOLDER):				{ Importer::Folders::Load(buffer, (R_Folder*)resource); break; }
			case (ResourceType::MESH):					{ Importer::Meshes::Load(buffer, (R_Mesh*)resource); break; }
			case (ResourceType::TEXTURE):				{ Importer::Textures::Load(buffer, size, (R_Texture*)resource); break; }
			case (ResourceType::MATERIAL):				{ Importer::Materials::Load(buffer, size, (R_Material*)resource); break; }
			case (ResourceType::MODEL):					{ Importer::Models::Load(buffer, (R_Model*)resource); break; }
			case (ResourceType::ANIMATION):				{ Importer::Animations::Load(buffer, (R_Animation*)resource); break; }
			case (ResourceType::ANIMATOR_CONTROLLER):	{ Importer::Animators::Load(buffer, (R_AnimatorController*)resource); break; }
			case (ResourceType::PARTICLESYSTEM):		{ Importer::Particles::Load(buffer, size, (R_ParticleSystem*)resource); break; }
			case (ResourceType::SHADER):				{ Importer::Shaders::Load(buffer, size, (R_Shader*)resource); break; }
			case (ResourceType::SCENE):					{ Importer::Scenes::Load(buffer, (R_Scene*)resource); break; }
		}
		RELEASE_ARRAY(buffer);
		resource->LoadOnMemory();
	}

	return resource;
}

void M_Resources::ReleaseResource(Resource* resource)
{
	resource->instances--;
	if (resource->instances < 0)
	{
		UnLoadResource(resource->GetID());
	}
}

ResourceType M_Resources::GetTypeFromFileExtension(const char* path) const
{
	std::string ext = "";
	Engine->fileSystem->SplitFilePath(path, nullptr, nullptr, &ext);

	static_assert(static_cast<int>(ResourceType::UNKNOWN) == 10, "Code Needs Update");

	if (ext == "FBX" || ext == "fbx")
		return ResourceType::MODEL;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "TGA" || ext == "PNG" || ext == "JPG")
		return ResourceType::TEXTURE;
	if (ext == "shader")
		return ResourceType::SHADER;
	if (ext == "particles")
		return ResourceType::PARTICLESYSTEM;
	if (ext == "shader")
		return ResourceType::SHADER;
	if (ext == "scene")
		return ResourceType::SCENE;
	if (ext == "anim")
		return ResourceType::ANIMATION;
	if (ext == "animator")
		return ResourceType::ANIMATOR_CONTROLLER;
	return ResourceType::UNKNOWN;
}

bool M_Resources::GetAllMetaFromType(ResourceType type, std::vector<const ResourceBase*>& metas) const
{
	std::map<uint64, ResourceBase>::const_iterator it;
	for (it = resourceLibrary.begin(); it != resourceLibrary.end(); ++it)
	{
		if ((*it).second.type == type)
			metas.push_back(&(*it).second);
	}
	return metas.size() > 0;
}

void M_Resources::LoadResourcesLibrary()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back("meta");

	PathNode engineAssets = Engine->fileSystem->GetAllFiles("Engine/Assets", &filter_ext);
	engineAssets.RemoveFoldersMetas();
	hEngineAssetsFolder.Set(LoadResourceInfoFromFolder(engineAssets));

	PathNode assets = Engine->fileSystem->GetAllFiles("Assets", &filter_ext);
	assets.RemoveFoldersMetas();
	hAssetsFolder.Set(LoadResourceInfoFromFolder(assets));
}

uint64 M_Resources::LoadResourceInfoFromFolder(PathNode node)
{
	uint64 resourceID = 0;
	resourceID = LoadResourceInfo(node.path.c_str());

	if (!node.isFile && !node.isLeaf)
	{
		//If node folder has something inside
		for (uint i = 0; i < node.children.size(); i++)
		{
			LoadResourceInfoFromFolder(node.children[i]);
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
		
		ResourceBase base((ResourceType)(int)(metaFile.GetNumber("Type")), metaFile.GetString("Assets File").c_str(), metaFile.GetString("Name").c_str(), metaFile.GetNumber("ID"));
		resourceID = base.ID;

		//Add all contained resources saved in the meta file
		Config_Array containingResources = metaFile.GetArray("Containing Resources");
		for (uint i = 0; i < containingResources.GetSize(); ++i)
		{
			Config contained = containingResources.GetNode(i);
			ResourceBase containedInfo((ResourceType)(int)(contained.GetNumber("Type")), base.assetsFile.c_str(), contained.GetString("Name").c_str(), contained.GetNumber("ID"));
			resourceLibrary[containedInfo.ID] = containedInfo;
		}
		resourceLibrary[resourceID] = base;

		RELEASE_ARRAY(buffer);
	}
	return resourceID;
}

//Save .meta file in assets
void M_Resources::SaveMetaInfo(const ResourceBase& base)
{
	Config config;
	base.Serialize(config);

	//Getting file modification date
	uint64 modDate = Engine->fileSystem->GetLastModTime(base.assetsFile.c_str());
	config.SetNumber("Date", modDate);

	Config_Array children = config.SetArray("Contained Resources");
	for (uint i = 0; i < base.containedResources.size(); ++i)
	{
		ResourceBase& baseChild = resourceLibrary[base.containedResources[i]];
		baseChild.Serialize(children.AddNode());
	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = base.assetsFile + ".meta";
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
	ResourceHandle<Resource> hFolder;
	bool isFolderUpdated = false;

	if (true) //TODO: ignoreResource
	{
		if (!Engine->fileSystem->Exists(std::string(node.path + ".meta").c_str()))
		{
			hFolder.Set(CreateNewResource(node.path.c_str(), ResourceType::FOLDER));
			isFolderUpdated = true;
		}
		else
		{
			hFolder.Set(GetResourceBase(node.path.c_str()).ID);
		}
	}

	for (uint i = 0; i < node.children.size(); i++)
	{
		uint64 childrenID = UpdateAssetsFolder(node.children[i]);
		if (childrenID != 0) //TODO: ignoreResource
		{
			hFolder.Get()->AddContainedResource(childrenID);
			isFolderUpdated = true;
		}
	}

	if (isFolderUpdated)
	{
		SaveResource(hFolder.Get());
		SaveMetaInfo(*hFolder.Get()->baseData);
	}

	return hFolder.GetID();
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
			SaveResource(it->second);
			it->second->needs_save = false;
		}
	}
}

uint M_Resources::DeleteResource(uint64 ID, bool deleteAsset)
{
	//TODO: update folder resource and remove this one from the contained list
	uint instances = 0;

	//Remove the resource if it is loaded
	std::map<uint64, Resource*>::iterator resourceIt = resources.find(ID);
	if (resourceIt != resources.end())
	{
		instances = resourceIt->second->instances;
		UnLoadResource(ID);
		resources.erase(ID);
	}
	
	//Remove the resource from the database.
	std::map<uint64, ResourceBase>::iterator libraryIt = resourceLibrary.find(ID);
	if (libraryIt != resourceLibrary.end())
	{
		if (deleteAsset)
		{
			Engine->fileSystem->Remove(libraryIt->second.assetsFile.c_str());
			Engine->fileSystem->Remove(libraryIt->second.assetsFile.append(".meta").c_str());
		}
		Engine->fileSystem->Remove(libraryIt->second.libraryFile.c_str());
		resourceLibrary.erase(libraryIt);
	}

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
		case(ResourceType::FOLDER):					{ size = Importer::Folders::Save((R_Folder*)resource, &buffer); break; }
		case(ResourceType::MESH):					{ size = Importer::Meshes::Save((R_Mesh*)resource, &buffer); break; }
		case(ResourceType::TEXTURE):				{ size = Importer::Textures::Save((R_Texture*)resource, &buffer); break; }
		case(ResourceType::MATERIAL):				{ size = Importer::Materials::Save((R_Material*)resource, &buffer); break; }
		case(ResourceType::ANIMATION):				{ size = Importer::Animations::Save((R_Animation*)resource, &buffer); break;	}
		case(ResourceType::ANIMATOR_CONTROLLER):	{ size = Importer::Animators::Save((R_AnimatorController*)resource, &buffer);	break; }
		case(ResourceType::MODEL):					{ size = Importer::Models::Save((R_Model*)resource, &buffer); break; }
		case(ResourceType::PARTICLESYSTEM):			{ size = Importer::Particles::Save((R_ParticleSystem*)resource, &buffer); break; }
		case(ResourceType::SHADER):					{ size = Importer::Shaders::Save((R_Shader*)resource, &buffer); break; }
		case(ResourceType::SCENE):					{ size = Importer::Scenes::Save((R_Scene*)resource, &buffer); break; }
	}

	if (size > 0)
	{
		Engine->fileSystem->Save(resource->GetLibraryFile(), buffer, size);

		if (IsModifiableResource(resource))
		{
			Engine->fileSystem->Save(resource->GetAssetsFile(), buffer, size);
			SaveMetaInfo(*resource->baseData);
		}
		RELEASE_ARRAY(buffer);
	}
}

uint64 M_Resources::SaveResourceAs(Resource* resource, const char* directory, const char* fileName)
{
	//TODO:   SaveResourceAs would override any existing resource with that name, and not remove its library content.

	//Creating a new empty resource just to add it into the database
	std::string path = directory + std::string("/").append(name);
	Resource* newResource = CreateNewResource(path.c_str(), resource->GetType(), fileName);

	//We save the content of the old resource with the baseData of the new resource and restore it later
	ResourceBase& oldBase = resourceLibrary[resource->GetID()];
	resource->baseData = newResource->baseData;

	SaveResource(resource);

	//Load the newly saved data into the new resource
	uint64 newID = newResource->GetID();
	UnLoadResource(newID);

	//Restore the old resource data
	resource->baseData = &oldBase;

	return newID;
}

bool M_Resources::IsModifiableResource(const Resource* resource) const
{
	return resource->isInternal == false;
}