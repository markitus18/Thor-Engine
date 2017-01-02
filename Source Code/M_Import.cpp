
#include "Application.h"
#include "M_Import.h"
#include "M_Scene.h"

#include "C_Material.h"
#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"

#include "M_Resources.h"

#include "Resource.h"
#include "R_Mesh.h"
#include "R_Material.h"

#include "Component.h"

#include "M_Materials.h"
#include "M_Meshes.h"
#include "M_Animations.h"

#include "M_FileSystem.h"
#include "M_Editor.h"

#include "Config.h"
#include "R_Prefab.h"
#include "Resource.h"
#include "M_Renderer3D.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

#include "MathGeoLib\src\MathGeoLib.h"
#include "M_Input.h"

#include <map>

M_Import::M_Import(bool start_enabled) : Module("Importer", start_enabled)
{}

// Destructor
M_Import::~M_Import()
{}

// Called before render is available
bool M_Import::Init(Config& config)
{
	LOG("Loading Module Import");

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

update_status M_Import::Update(float dt)
{
	return UPDATE_CONTINUE;
}

void M_Import::SaveGameObjectConfig(Config& config, std::vector<GameObject*>& gameObjects)
{
	Config_Array array = config.SetArray("GameObjects");

	for (uint i = 0; i < gameObjects.size(); i++)
	{
		SaveGameObjectSingle(array.AddNode(), gameObjects[i]);
	}
}

void M_Import::SaveGameObjectComponent(Config& config, Component* component)
{
	config.SetBool("HasResource", component->HasResource());
	if (component->HasResource())
	{
		Resource* resource = component->GetResource();
		if (resource != nullptr)
		{
			config.SetNumber("Type", static_cast<int>(resource->GetType()));
			config.SetNumber("ID", resource->GetID());
			config.SetString("ResourceName", resource->GetName());
		}
	}
}

GameObject* M_Import::LoadGameObject(uint64 ID)
{
	std::string full_path = "Library/GameObjects/";
	full_path.append(std::to_string(ID));// .append(".mesh");

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		Config config(buffer);
		std::vector<GameObject*> roots;
		LoadGameObjectConfig(config, roots);
		return roots[0];
	}

	return nullptr;
}

void M_Import::LoadGameObjectConfig(Config& config, std::vector<GameObject*>& roots)
{
	std::vector<GameObject*> not_parented_GameObjects;
	std::map<unsigned long long, GameObject*> createdGameObjects;

	GameObject* ret = nullptr;

	Config_Array gameObjects_array = config.GetArray("GameObjects");
	for (uint i = 0; i < gameObjects_array.GetSize(); i++)
	{
		//Single GameObject load
		Config gameObject_node = gameObjects_array.GetNode(i);

		float3 position = gameObject_node.GetArray("Translation").GetFloat3(0);
		Quat rotation = gameObject_node.GetArray("Rotation").GetQuat(0);
		float3 scale = gameObject_node.GetArray("Scale").GetFloat3(0);

		//Parent setup
		GameObject* parent = nullptr;
		std::map<unsigned long long, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		GameObject* gameObject = new GameObject(parent, gameObject_node.GetString("Name").c_str(), position, scale, rotation);
		gameObject->uid = gameObject_node.GetNumber("UID");
		createdGameObjects[gameObject->uid] = gameObject;

		if (gameObject_node.GetNumber("ParentUID") == 0)
		{
			roots.push_back(gameObject);
		}

		else if (parent == nullptr)
			not_parented_GameObjects.push_back(gameObject);

		gameObject->active = gameObject_node.GetBool("Active");
		gameObject->isStatic = gameObject_node.GetBool("Static");
		bool selected = gameObject_node.GetBool("Selected", false);
		if (selected)
		{
			App->moduleEditor->AddSelect(gameObject);
		}

		gameObject->beenSelected = gameObject->hierarchyOpen = gameObject_node.GetBool("OpenInHierarchy", false);

		Config_Array components = gameObject_node.GetArray("Components");

		for (uint i = 0; i < components.GetSize(); i++)
		{
			Config component = components.GetNode(i);
			if (component.GetBool("HasResource") == true)
			{
				Resource* resource = App->moduleResources->GetResource(component.GetNumber("ID"));
				if (resource)
				{
					Component::Type type = App->moduleResources->ResourceToComponentType(resource->type);
					if (type != Component::None)
					{
						Component* component = gameObject->CreateComponent(type);
						if (component)
							component->SetResource(resource->GetID());
					}
				}
			}
		}

		bool camera = gameObject_node.GetNumber("Camera");
		if (camera == true)
		{
			gameObject->CreateComponent(Component::Camera);
		}

		//Call OnUpdateTransform() to init all components according to the GameObject
		gameObject->OnUpdateTransform();
	}

	//After all GameObjects have been loaded, animation links are created
	std::map<unsigned long long, GameObject*>::iterator it;
	for (it = createdGameObjects.begin(); it != createdGameObjects.end(); it++)
	{
		C_Animation* anim = (C_Animation*)it->second->GetComponent<C_Animation>();
		if (anim != nullptr)
			anim->LinkChannels();
	}
	//Security method if any game object is left without a parent
	for (uint i = 0; i < not_parented_GameObjects.size(); i++)
	{
		LOG("[warning] GameObject not parented when loading prefab");
		roots.push_back(not_parented_GameObjects[i]);
	}
}

R_Prefab* M_Import::LoadPrefabResource(uint64 ID)
{
	std::string full_path = ("/Library/GameObjects/");
	full_path.append(std::to_string(ID));

	R_Prefab* prefab = nullptr;

	char* buffer = nullptr;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);
	if (size > 0)
	{
		Config config(buffer);

		prefab = new R_Prefab();
		prefab->ID = ID;
		prefab->resource_file = full_path.c_str();
		prefab->original_file = config.GetString("Source");
		prefab->name = prefab->original_file;
		prefab->miniTextureID = config.GetNumber("MiniTexture");
	}
	return prefab;
}

void M_Import::SaveGameObjectSingle(Config& config, GameObject* gameObject)
{
	config.SetNumber("UID", gameObject->uid);

	config.SetNumber("ParentUID", gameObject->parent ? gameObject->parent->uid : 0);
	config.SetString("Name", gameObject->name.c_str());

	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	//Translation part
	Config_Array transform_node = config.SetArray("Translation");
	transform_node.AddFloat3(transform->GetPosition());

	//Rotation part
	Config_Array rotation_node = config.SetArray("Rotation");
	rotation_node.AddQuat(transform->GetQuatRotation());

	//Scale part
	Config_Array scale_node = config.SetArray("Scale");
	scale_node.AddFloat3(transform->GetScale());

	config.SetBool("Active", gameObject->active);
	config.SetBool("Static", gameObject->isStatic);
	config.SetBool("Selected", gameObject->IsSelected());
	config.SetBool("OpenInHierarchy", gameObject->hierarchyOpen);

	Config_Array compConfig = config.SetArray("Components");
	const std::vector<Component*> components = gameObject->GetAllComponents();

	for (uint i = 0; i < components.size(); i++)
	{
		SaveGameObjectComponent(compConfig.AddNode(), components[i]);
	}


	C_Camera* cam = gameObject->GetComponent<C_Camera>();
	if (cam)
	{
		config.SetNumber("Camera", 1);
	}
	else
	{
		config.SetNumber("Camera", 0);
	}
}

R_Prefab* M_Import::ImportFile(const char* path, Uint32 ID)
{
	R_Prefab* ret = nullptr;

	const aiScene* file = aiImportFileEx(path, aiProcessPreset_TargetRealtime_MaxQuality, App->fileSystem->GetAssimpIO());
	if (file)
	{
		LOG("Starting scene load %s", path);
		std::vector<GameObject*> createdGameObjects;
		std::vector<const aiMesh*> bonedMeshes;

		GameObject* rootNode = CreateGameObjects(file, file->mRootNode, nullptr, path, createdGameObjects, bonedMeshes);
		uint64 animID = App->moduleAnimations->ImportSceneAnimations(file, rootNode, path);
		if (animID != 0)
		{
			C_Animation* animation = (C_Animation*)rootNode->CreateComponent(Component::Type::Animation);
			animation->SetResource(animID);
		}

		Config config;
		SaveGameObjectConfig(config, createdGameObjects);

		//Saving mini-texture
		uint miniTexID = 0;
		miniTexID = App->renderer3D->SavePrefabImage(rootNode);

		config.SetNumber("MiniTexture", miniTexID);
		config.SetString("Name", path);
		config.SetString("Source", path);

		char* buffer;
		uint size = config.Serialize(&buffer);

		if (size > 0)
		{
			std::string fileName;
			App->fileSystem->SplitFilePath(path, nullptr, &fileName);

			std::string full_path = "/Library/GameObjects/";
			full_path.append(std::to_string(ID));
			App->fileSystem->Save(full_path.c_str(), buffer, size);

			ret = new R_Prefab();
			ret->ID = ID;
			ret->resource_file = full_path.c_str();
			ret->original_file = path;
			ret->name = fileName;
			ret->miniTextureID = miniTexID;

			RELEASE_ARRAY(buffer);
		}

		aiReleaseImport(file);
		RELEASE(rootNode);
	}
	else
	{
		LOG("File not found: %s", aiGetErrorString());
	}

	return ret;
}

GameObject* M_Import::CreateGameObjects(const aiScene* scene, const aiNode* node, GameObject* parent, const char* path, std::vector<GameObject*>& vector, std::vector<const aiMesh*>& bonedMeshes)
{
	aiVector3D		translation;
	aiVector3D		scaling;
	aiQuaternion	rotation;
	
	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	
	//Skipp all dummy modules. Assimp loads this fbx nodes to stack all transformations
	std::string node_name = node->mName.C_Str();	

	bool dummyFound = true;
	while (dummyFound)
	{
		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (node_name.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next GameObject
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(scaling, rotation, translation);
			pos += float3(translation.x, translation.y, translation.z);
			scale = float3(scale.x * scaling.x, scale.y * scaling.y, scale.z * scaling.z);
			rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			node_name = node->mName.C_Str();

			//if we find a dummy node we "change" our current node into the dummy one and search
			//for other dummy nodes inside that one.
			dummyFound = true;
		}
		else
			dummyFound = false;
	}
	
	//Cutting path into file name --------------------
	if (node_name == "RootNode")
	{
		node_name = path;
		uint slashPos;
		if ((slashPos = node_name.find_last_of("/")) != std::string::npos)
			node_name = node_name.substr(slashPos + 1, node_name.size() - slashPos);

		uint pointPos;
		if ((pointPos = node_name.find_first_of(".")) != std::string::npos)
			node_name = node_name.substr(0, node_name.size() - (node_name.size() - pointPos));
	}
	//-----------------------------------------------

	GameObject* gameObject = new GameObject(parent, node_name.c_str(), pos, scale, rot);
	gameObject->uid = random.Int();
	vector.push_back(gameObject);

	// Loading node meshes ----------------------------------------
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* newMesh = scene->mMeshes[node->mMeshes[i]];

		if (newMesh->HasBones())
			bonedMeshes.push_back(newMesh);

		GameObject* child = nullptr;

		if (node->mNumMeshes > 1)
		{
			node_name = newMesh->mName.C_Str();
			if (node_name == "")
				node_name = gameObject->name + "_submesh";
			if (i > 0)
				node_name.append("(" +std::to_string(i) + ")");
			child = new GameObject(gameObject, node_name.c_str());
			child->uid = random.Int();
			vector.push_back(child);
		}
		else
		{
			child = gameObject;
		}

		uint64 rMesh= App->moduleResources->ImportRMesh(newMesh, path, child->name.c_str());
		if (rMesh != 0)
		{
			C_Mesh* cMesh = (C_Mesh*)child->CreateComponent(Component::Mesh);
			cMesh->SetResource(rMesh);
		}

		//Loading mesh materials ---------
		aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
		aiString matName;
		material->Get(AI_MATKEY_NAME, matName);
		uint64 rMaterial = App->moduleResources->ImportRMaterial(material, path, matName.C_Str());
		if (rMaterial != 0)
		{
			C_Material* cMaterial = new C_Material(nullptr);
			cMaterial->SetResource(rMaterial);
			child->AddComponent(cMaterial);
		}
		//--------------------------------

	}
	// ------------------------------------------------------------
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		GameObject* new_child = CreateGameObjects(scene, node->mChildren[i], gameObject, path, vector, bonedMeshes);
	}
	return gameObject;
}

// Called before quitting
bool M_Import::CleanUp()
{
	return true;
}
