#include "M_Resources.h"
#include "Application.h"

//Loaders
#include "M_Meshes.h"
#include "M_Materials.h"
#include "M_Import.h"
#include "M_Animations.h"
#include "M_ParticleSystems.h"
#include "M_Shaders.h"

//Resources
#include "R_Mesh.h"
#include "R_Texture.h"
#include "R_Material.h"
#include "R_Prefab.h"
#include "R_Animation.h"
#include "R_Bone.h"
#include "R_ParticleSystem.h"
#include "R_Shader.h"

#include "M_FileSystem.h"
#include "PathNode.h"

#include "Config.h"
#include "M_Scene.h"
#include "M_Editor.h"
#include "W_Explorer.h"

//TODO: Can we include assimp only in meshes importer? Import Scene is using it now
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

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
	Importer::Scenes::Init();

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

void M_Resources::ImportFileFromExplorer(const char* path)
{
	std::string normalizedPath = App->fileSystem->NormalizePath(path);
	std::string finalPath;
	if (App->fileSystem->DuplicateFile(normalizedPath.c_str(), App->moduleEditor->w_explorer->GetCurrentNode().path.c_str(), finalPath))
	{
		ImportFileFromAssets(finalPath.c_str());
	}
}

Resource* M_Resources::ImportFileFromAssets(const char* path)
{
	Resource::Type type = GetTypeFromPath(path);
	Resource* resource = CreateResourceBase(path, type);

	//TODO: Build resource name
	//TODO: check override when re-importing
	if (ResourceMeta* meta = FindResourceInLibrary(path, resource->name.c_str(), type))
		resource->ID = meta->id;
	/*
	//Find if the resource already exists and delete it
	ResourceMeta* meta = FindResourceInLibrary(source_file, name, Resource::MATERIAL);
	if (meta != nullptr)
	{
		newID = meta->id;
		instances = DeleteResource(newID);
	}
	else
	{
		newID = GetNewID();
	}*/

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(path, &buffer);

	switch (type)
	{
		case (Resource::TEXTURE):
		{
			ImportRTexture(buffer, size, (R_Texture*)resource);
			break;
		}
		case (Resource::PREFAB):
		{
			ImportScene(buffer, size, (R_Prefab*)resource);
			break;
		}
		case(Resource::SHADER):
		{
			ImportRShader(buffer, size, (R_Shader*)resource);
			break;
		}
	}

	RELEASE_ARRAY(buffer);

	AddResource(resource);
	return resource;
}

void M_Resources::ImportScene(const char* buffer, uint size, R_Prefab* prefab)
{
	const aiScene* scene = Importer::Scenes::ProcessAssimpScene(buffer, size);
	GameObject* root = Importer::Scenes::Import(scene);
	std::vector<uint64> meshes, materials;

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		//TODO: building a temporal mesh name. Assimp meshes have no name so they have conflict when importing multiple meshes
		std::string meshName = prefab->name.append("_mesh").append(std::to_string(i));
		scene->mMeshes[i]->mName = meshName;
		meshes.push_back(ImportRMesh(scene->mMeshes[i], prefab->original_file.c_str()));
	}
	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		materials.push_back(ImportRMaterial(scene->mMaterials[i], prefab->original_file.c_str()));
	}

	//Link all loaded meshes and materials to the existing gameObjects
	Importer::Scenes::LinkSceneResources(root, meshes, materials);

	Config file;
	Importer::Scenes::SaveScene(root, file);

	char* saveBuffer = nullptr;
	if (uint saveSize = file.Serialize(&saveBuffer))
		App->fileSystem->Save(prefab->resource_file.c_str(), saveBuffer, saveSize);
}

uint64 M_Resources::ImportRMesh(const aiMesh* mesh, const char* source_file)
{
	Resource* resource = CreateResourceBase(source_file, Resource::Type::MESH, mesh->mName.C_Str());

	if (ResourceMeta* meta = FindResourceInLibrary(source_file, mesh->mName.C_Str(), Resource::Type::MESH))
		resource->ID = meta->id;

	Importer::Meshes::Import(mesh, (R_Mesh*)resource);

	char* saveBuffer = nullptr;
	if (uint64 saveSize = Importer::Meshes::Save((R_Mesh*)resource, &saveBuffer))
	{
		App->fileSystem->Save(resource->resource_file.c_str(), saveBuffer, saveSize);
	}

	AddResource(resource);
	return resource->ID;
}

void M_Resources::ImportRTexture(const char* buffer, uint size, R_Texture* texture)
{
	//Importing resource
	if (Importer::Textures::Import(buffer, size, texture))
	{
		char* saveBuffer = nullptr;
		if (uint saveSize = Importer::Textures::Save(texture, &saveBuffer))
			App->fileSystem->Save(texture->resource_file.c_str(), saveBuffer, saveSize);
	}
}

uint64 M_Resources::ImportPrefabImage(char* buffer, const char* source_file, uint sizeX, uint sizeY)
{
	uint64 ret = 0;
	uint64 newID = 0;
	R_Texture* resource = nullptr;
	uint64 instances = 0;

	std::string name = "", extension = "";
	ResourceMeta* meta = FindResourceInLibrary(source_file, name.c_str(), Resource::TEXTURE);

	if (meta != nullptr)
	{
		newID = meta->id;
		instances = DeleteResource(newID);
	}
	else
	{
		newID = GetNewID();
	}

	//Importing resource
	//resource = App->moduleMaterials->ImportPrefabImage(buffer, newID, source_file, sizeX, sizeY);
	if (resource)
	{
		resource->instances = instances;
		AddResource(resource);
		ret = resource->ID;
	}
	return ret;
}

uint64 M_Resources::ImportRMaterial(const aiMaterial* mat, const char* source_file)
{
	aiString matName;
	mat->Get(AI_MATKEY_NAME, matName);
	Resource* resource = CreateResourceBase(source_file, Resource::Type::MATERIAL, matName.C_Str());

	if (ResourceMeta* meta = FindResourceInLibrary(source_file, matName.C_Str(), Resource::Type::MATERIAL))
		resource->ID = meta->id;

	Importer::Materials::Import(mat, (R_Material*)resource);

	char* saveBuffer = nullptr;
	if (uint64 saveSize = Importer::Materials::Save((R_Material*)resource, &saveBuffer))
	{
		App->fileSystem->Save(resource->resource_file.c_str(), saveBuffer, saveSize);
	}

	AddResource(resource);
	return resource->ID;
}

uint64 M_Resources::ImportRAnimation(const aiAnimation* anim, const char* source_file, const char* name)
{
	uint64 ret = 0;
	uint64 newID = 0;
	R_Animation* resource = nullptr;
	uint64 instances = 0;

	//Find if the resource already exists and delete it
	ResourceMeta* meta = FindResourceInLibrary(source_file, name, Resource::ANIMATION);
	if (meta != nullptr)
	{
		newID = meta->id;
		instances = DeleteResource(newID);
	}
	else
	{
		newID = GetNewID();
	}

	//Importing resource
	resource = App->moduleAnimations->ImportAnimation(anim, newID, source_file);
	if (resource)
	{
		resource->instances = instances;
		AddResource(resource);
		ret = resource->ID;
	}
	return ret;
}

uint64 M_Resources::ImportRBone(const aiBone* bone, const char* source_file, const char* name, uint64 meshID)
{
	uint64 ret = 0;
	uint64 newID = 0;
	R_Bone* resource = nullptr;
	uint64 instances = 0;

	//Find if the resource already exists and delete it
	ResourceMeta* meta = FindResourceInLibrary(source_file, name, Resource::BONE);
	if (meta != nullptr)
	{
		newID = meta->id;
		instances = DeleteResource(newID);
	}
	else
	{
		newID = random.Int();
	}

	//Importing resource
	resource = App->moduleAnimations->ImportBone(bone, newID, source_file, meshID);
	if (resource)
	{
		resource->instances = instances;
		AddResource(resource);
		ret = resource->ID;
	}
	return ret;
}

uint64 M_Resources::ImportRParticleSystem(const char* assetsPath)
{
	return 0;
}

void M_Resources::ImportRShader(const char* buffer, uint size, R_Shader* shader)
{
	Importer::Shaders::Import(buffer, shader);

	//TODO: This will be done once binary shaders are working. By now we simply duplicate the file
	/*
	char* saveBuffer = nullptr;
	if (uint64 saveSize = Importer::Shaders::Save((R_Shader*)shader, &saveBuffer))
	{
		App->fileSystem->Save(shader->resource_file.c_str(), saveBuffer, saveSize);
	}
	*/

	App->fileSystem->Save(shader->resource_file.c_str(), buffer, size);

	AddResource(shader);
}

Resource* M_Resources::CreateResourceBase(const char* assetsPath, Resource::Type type, const char* name, uint64 forceID)
{
	Resource* resource = nullptr;
	std::string resourcePath;

	switch (type)
	{
		case Resource::FOLDER:
			break;
		case Resource::MESH:
			resource = new R_Mesh();
			resource->resource_file = MESHES_PATH;
			break;
		case Resource::TEXTURE:
			resource = new R_Texture();
			resource->resource_file = TEXTURES_PATH;
			break;
		case Resource::MATERIAL:
			resource = new R_Material();
			resource->resource_file = MATERIALS_PATH;
			break;
		case Resource::ANIMATION:
			resource = new R_Animation();
			resource->resource_file = ANIMATIONS_PATH;
			break;
		case Resource::BONE:
			resource = new R_Bone();
			resource->resource_file = BONES_PATH;
			break;
		case Resource::PREFAB:
			resource = new R_Prefab();
			resource->resource_file = GAMEOBJECTS_PATH;
			break;
		case Resource::PARTICLESYSTEM:
			resource = new R_ParticleSystem();
			resource->resource_file = PARTICLES_PATH;
			break;
		case Resource::SHADER:
			resource = new R_Shader();
			resource->resource_file = SHADERS_PATH;
			break;
		case Resource::SCENE:
			resource = new Resource(Resource::SCENE);
	}

	if (resource == nullptr) return nullptr;

	if (name == nullptr)
	{
		std::string extension;
		App->fileSystem->SplitFilePath(assetsPath, nullptr, &resource->name, &extension);
		if (type == Resource::TEXTURE) resource->name.append(extension);
	}
	else
		resource->name = name;

	resource->ID = forceID ? forceID : GetNewID();
	resource->original_file = assetsPath;
	resource->resource_file.append(std::to_string(resource->ID));

	return resource;
}

Resource* M_Resources::CreateNewResource(const char* assetsPath, Resource::Type type)
{
	Resource* ret = nullptr;
	switch (type)
	{
		case(Resource::Type::PARTICLESYSTEM):
		{	
			ret = App->moduleParticleSystems->CreateNewParticleSystem(assetsPath, random.Int());

		}
	}

	if (ret != nullptr)
		AddResource(ret);

	return ret;
}

Resource* M_Resources::LoadResourceBase(uint64 ID)
{
	Resource* resource = nullptr;
	std::map<uint64, ResourceMeta>::iterator metasIt = existingResources.find(ID);
	if (metasIt != existingResources.end())
	{
		ResourceMeta* metaInfo = &metasIt->second;
		resource = CreateResourceBase(metaInfo->original_file.c_str(), metaInfo->type, metasIt->second.resource_name.c_str(), ID);
	}
	return resource;
}

Resource* M_Resources::GetResource(uint64 ID)
{
	Resource* resource = nullptr;
	//First find if the wanted resource is loaded
	std::map<uint64, Resource*>::iterator resourcesIt = resources.find(ID);
	if (resourcesIt != resources.end()) return resourcesIt->second;

	if (resource = LoadResourceBase(ID))
	{
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(resource->resource_file.c_str(), &buffer);
		if (size == 0)
		{
			RELEASE(resource);
			return nullptr;
		}

		switch (resource->type)
		{
			case (Resource::MESH):
			{
				Importer::Meshes::Load(buffer, (R_Mesh*)resource);
				break;
			}
			case (Resource::TEXTURE):
			{
				Importer::Textures::Load(buffer, size, (R_Texture*)resource);
				break;
			}
			case (Resource::MATERIAL):
			{
				Importer::Materials::Load(buffer, size, (R_Material*)resource);
				break;
			}
			case (Resource::PREFAB):
			{
				break;
			}
			case (Resource::ANIMATION):
			{
				//resource = App->moduleAnimations->LoadAnimation(ID);
				break;
			}
			case (Resource::BONE):
			{
				//resource = App->moduleAnimations->LoadBone(ID);
				break;
			}
			case (Resource::PARTICLESYSTEM):
			{
				resource = App->moduleParticleSystems->LoadParticleSystemResource(ID);
				break;
			}
			case (Resource::SHADER):
			{
				Importer::Shaders::Load(buffer, size, (R_Shader*)resource);
				break;
			}
		}
	}
	if (resource) LoadResource(resource);
	return resource;
}

Resource::Type M_Resources::GetTypeFromPath(const char* path) const
{
	std::string ext = "";
	App->fileSystem->SplitFilePath(path, nullptr, nullptr, &ext);

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
	return Resource::UNKNOWN;
}

bool M_Resources::GetAllMetaFromType(Resource::Type type, std::vector<const ResourceMeta*>& metas) const
{
	std::map<uint64, ResourceMeta>::const_iterator it;
	for (it = existingResources.begin(); it != existingResources.end(); ++it)
	{
		if ((*it).second.type == type)
			metas.push_back(&(*it).second);
	}
	return metas.size() > 0;
}

void M_Resources::LoadPrefab(uint64 ID)
{
	if (Resource* resource = GetResource(ID))
	{
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(resource->resource_file.c_str(), &buffer);
		if (size > 0)
		{
			Config file(buffer);
			App->scene->LoadGameObject(file);
		}

		resource->instances++;
	}

	LOG("Could not find file with ID '%i' loaded in resources", ID);
}

PathNode M_Resources::CollectImportedScenes()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back("FBX");
	filter_ext.push_back("fbx");

	return App->fileSystem->GetAllFiles("Assets", &filter_ext);
}


Component::Type M_Resources::ResourceToComponentType(Resource::Type type)
{
	switch (type)
	{
	case Resource::MESH: return Component::Mesh;
	case Resource::MATERIAL: return Component::Material;
	case Resource::ANIMATION: return Component::Animation;
	case Resource::BONE: return Component::Bone;
	default: return Component::None;
	}
}

void M_Resources::LoadResourcesData()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back("meta");

	PathNode assets = App->fileSystem->GetAllFiles("Assets", &filter_ext);
	LoadMetaFromFolder(assets);

	//TODO: check what this bunch of code does
	//Just to save mini-tex images
 	PathNode tex = App->fileSystem->GetAllFiles("Library/Textures", &filter_ext);
	LoadMetaFromFolder(tex);
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

ResourceMeta* M_Resources::FindResourceInLibrary(const char* original_file, const char* name, Resource::Type type)
{
	for (std::map<uint64, ResourceMeta>::iterator it = existingResources.begin(); it != existingResources.end(); it++)
	{
		if (it->second.Compare(original_file, name, type))
		{
			return &(it->second);
		}
	}
	return nullptr;
}

ResourceMeta M_Resources::GetMetaInfo(const Resource* resource)
{
	ResourceMeta ret;

	ret.original_file = resource->original_file;
	ret.resource_name = resource->name;
	ret.type = resource->type;
	ret.id = resource->ID;

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

		std::string sourceFile = file;
		sourceFile = std::string(file).substr(0, sourceFile.size() - 5);

		meta.original_file = sourceFile;
		meta.resource_name = config.GetString("Name");
		meta.id = config.GetNumber("ID");
		meta.type = static_cast<Resource::Type>((int)(config.GetNumber("Type")));
		existingResources[meta.id] = meta;

		if (meta.type == Resource::PREFAB)
		{
			std::string resFile = "/Library/GameObjects/";
			resFile.append(std::to_string(meta.id));

			LoadSceneMeta(resFile.c_str(), sourceFile.c_str());
		}

		delete buffer;
		return true;
	}
	return false;
}

bool M_Resources::LoadSceneMeta(const char* file, const char* source_file)
{
	char* buffer = nullptr;
	uint size = App->fileSystem->Load(file, &buffer);
	if (size > 0)
	{
		Config config(buffer);
		Config_Array GameObjects = config.GetArray("GameObjects");

		for (uint i = 0; i < GameObjects.GetSize(); i++)
		{
			Config_Array components = GameObjects.GetNode(i).GetArray("Components");

			for (uint i = 0; i < components.GetSize(); i++)
			{
				Config resource = components.GetNode(i);
				if (resource.GetBool("HasResource"))
				{
					ResourceMeta meta;
					meta.id = resource.GetNumber("ID");
					meta.type = static_cast<Resource::Type>((int)resource.GetNumber("Type"));
					meta.resource_name = resource.GetString("ResourceName");
					meta.original_file = source_file;
					existingResources[meta.id] = meta;
				}
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

	config.SetNumber("ID", resource->ID);
	config.SetString("Name", resource->name.c_str());
	config.SetNumber("Type", static_cast<int>(resource->GetType()));

	//Getting file modification date
	uint64 modDate = App->fileSystem->GetLastModTime(resource->original_file.c_str());
	config.SetNumber("Date", modDate);

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = resource->original_file + ".meta";
		App->fileSystem->Save(path.c_str(), buffer, size);
		delete buffer;
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
		else
		{
			if (IsFileModified(node.path.c_str()))
			{
				LOG("File modified: %s", node.path.c_str());
				std::string meta_file = node.path + (".meta");
				//uint64 id = GetIDFromMeta(meta_file.c_str());
				ImportFileFromAssets(node.path.c_str());// , id);
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
		uint64 configData = Config(buffer).GetNumber("Date");
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
		ret = Config(buffer).GetNumber("ID");

	return ret;
}

Resource::Type M_Resources::GetTypeFromMeta(const char* path)
{
	Resource::Type ret = Resource::Type::UNKNOWN;

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(path, &buffer);

	if (size > 0)
		ret = (Resource::Type)(int)Config(buffer).GetNumber("Type");

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
					char* buffer = nullptr;
					if (uint size = Importer::Materials::Save((R_Material*)it->second, &buffer))
					{
						App->fileSystem->Save(it->second->resource_file.c_str(), buffer, size);
						RELEASE(buffer);
					}
				}
			}
			it->second->needs_save = false;
		}
	}
}

void M_Resources::AddResource(Resource* resource)
{
	//TODO: this will break, setup like this just to compile
	LoadResource(resource);
	existingResources[resource->ID] = GetMetaInfo(resource);
	SaveMetaInfo(resource);
}

void M_Resources::LoadResource(Resource* resource)
{
	if (resource == nullptr)
	{
		LOG("Error: Trying to load a null resource");
		return;
	}

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
	case(Resource::ANIMATION):
	{
		animations[resource->ID] = resource;
		break;
	}
	}
	resource->LoadOnMemory();
}

uint M_Resources::DeleteResource(uint64 ID)
{
	Resource::Type type = existingResources[ID].type;
	uint instances = 0;

	if (resources[ID] != nullptr)
	{
		//Could break here
		instances = resources[ID]->instances;
		UnLoadResource(ID);
		RELEASE(resources[ID]);
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
	App->fileSystem->Remove(resourcePath.c_str());

	existingResources.erase(ID);
	return instances;
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