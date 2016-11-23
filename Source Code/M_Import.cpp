
#include "Application.h"
#include "M_Import.h"
#include "M_Scene.h"

#include "C_Material.h"
#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"

#include "M_Materials.h"
#include "M_Meshes.h"
#include "M_FileSystem.h"
#include "Config.h"

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

#include <unordered_map>

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

GameObject* M_Import::LoadGameObject(const char* path)
{
	std::string full_path = "Library/GameObjects/";
	full_path.append(path);// .append(".mesh");

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
	std::unordered_map<unsigned long long, GameObject*> createdGameObjects;

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
		std::unordered_map<unsigned long long, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		GameObject* gameObject = new GameObject(parent, gameObject_node.GetString("Name").c_str(), position, scale, rotation);
		gameObject->uid = gameObject_node.GetNumber("UID");
		createdGameObjects.insert(std::pair<uint, GameObject*>(gameObject->uid, gameObject));

		if (gameObject_node.GetNumber("ParentUID") == 0)
		{
			roots.push_back(gameObject);
		}

		if (parent == nullptr)
			not_parented_GameObjects.push_back(gameObject);

		gameObject->active = gameObject_node.GetBool("Active");
		gameObject->isStatic = gameObject_node.GetBool("Static");
		gameObject->beenSelected = gameObject->hierarchyOpen = gameObject_node.GetBool("OpenInHierarchy", false);

		//Mesh load
		std::string meshPath = gameObject_node.GetString("Mesh");

		if (meshPath != "")
		{
			C_Mesh* mesh = App->moduleMeshes->LoadMesh(meshPath.c_str());
			if (mesh != nullptr)
			{
				gameObject->AddComponent(mesh);
			}
		}

		//Material load
		std::string matPath = gameObject_node.GetString("Material");

		if (matPath != "")
		{
			C_Material* mat = App->moduleMaterials->LoadMaterial(matPath.c_str());
			if (mat != nullptr)
			{
				gameObject->AddComponent(mat);
			}
		}
	}

	//Security method if any game object is left without a parent
	for (uint i = 0; i < not_parented_GameObjects.size(); i++)
	{
		LOG("[warning] GameObject not parented when loading prefab");
		//not_parented_GameObjects[i]->parent = root;
	}
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
	config.SetBool("OpenInHierarchy", gameObject->hierarchyOpen);

	//A thought: each component type will have a folder, same number as their enumeration
	//Transform = 01 // Mesh = 02 // Material = 03 ...
	//Each component will go indexed by a number also, not a file name: mesh path would be Library/02/02.mesh

	//TMP for mesh storage
	std::string meshLibFile = "";
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	if (mesh)
		meshLibFile = mesh->libFile;
	config.SetString("Mesh", meshLibFile.c_str());

	std::string matLibFile = "";
	C_Material* mat = gameObject->GetComponent<C_Material>();
	if (mat)
		matLibFile = mat->libFile;
	config.SetString("Material", matLibFile.c_str());
}

void M_Import::ImportFile(char* path)
{
	LOG("Entering mesh load");
	const aiScene* file = aiImportFileEx(path, aiProcessPreset_TargetRealtime_MaxQuality, App->fileSystem->GetAssimpIO());
	if (file)
	{
		LOG("Starting scene load %s", path);
		std::vector<GameObject*> createdGameObjects;
		GameObject* rootNode = LoadFBX(file, file->mRootNode, nullptr, path, createdGameObjects);
		
		Config config;
		SaveGameObjectConfig(config, createdGameObjects);

		char* buffer;
		uint size = config.Serialize(&buffer);

		if (size > 0)
		{
			std::string fileName;
			App->fileSystem->SplitFilePath(path, nullptr, &fileName);

			std::string full_path = "/Library/GameObjects/";
			full_path.append(fileName);
			App->fileSystem->Save(full_path.c_str(), buffer, size);

			RELEASE_ARRAY(buffer);
		}

		aiReleaseImport(file);
		RELEASE(rootNode);
	}
	else
	{
		LOG("File not found");
	}
}

GameObject* M_Import::LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path, std::vector<GameObject*>& vector)
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
		if (node_name.find("$AssimpFbx$") != std::string::npos && node->mNumChildren == 1)
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
		GameObject* child = nullptr;

		if (node->mNumMeshes > 1)
		{
			node_name = newMesh->mName.C_Str();
			if (node_name == "")
				node_name = "No name";
			child = new GameObject(gameObject, node_name.c_str());
			child->uid = random.Int();
			vector.push_back(child);
		}
		else
		{
			child = gameObject;
		}

		C_Mesh* mesh = App->moduleMeshes->ImportMesh(newMesh, node_name.c_str());
		child->AddComponent(mesh);


		//Loading mesh materials ---------
		aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
		C_Material* go_mat = App->moduleMaterials->ImportMaterial(material, path);
		child->AddComponent(go_mat);
		//--------------------------------

	}
	// ------------------------------------------------------------
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		GameObject* new_child = LoadFBX(scene, node->mChildren[i], gameObject, path, vector);
	}
	return gameObject;
}

// Called before quitting
bool M_Import::CleanUp()
{
	return true;
}
