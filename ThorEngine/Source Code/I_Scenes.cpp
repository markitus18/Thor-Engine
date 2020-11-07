#include "I_Scenes.h"

#include "GameObject.h"

#include "Component.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Camera.h"
#include "C_Animator.h"
#include "C_ParticleSystem.h"

#include "Resource.h"
#include "R_Model.h"
#include "R_Map.h"

#include "Config.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "MathGeoLib\src\MathGeoLib.h" //TODO: Include ONLY the necessary structures

namespace Importer { namespace Models { LCG randomID; } }

R_Model* Importer::Models::Create()
{
	return new R_Model();
}

const aiScene* Importer::Models::ProcessAssimpScene(const char* buffer, uint size)
{
	return aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
}

void Importer::Models::Import(const aiScene* scene, R_Model* model)
{
	Private::ImportNodeData(scene, scene->mRootNode, model, 0);
	(*model->nodes.begin()).name = model->GetName();
}

void Importer::Models::Private::ImportNodeData(const aiScene* scene, const aiNode* node, R_Model* model, uint64 parentID)
{
	aiVector3D		aiTranslation, aiScale;
	aiQuaternion	aiRotation;

	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);

	float3 pos(aiTranslation.x, aiTranslation.y, aiTranslation.z);
	float3 scale(aiScale.x, aiScale.y, aiScale.z);
	Quat rot(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

	//Assimp loads "dummy" modules to stack fbx transformation. Here we collapse all those transformations
	//to the first node that is not a dummy
	std::string nodeName = node->mName.C_Str();
	bool dummyFound = true;
	while (dummyFound)
	{
		dummyFound = false;

		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (nodeName.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next node
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);
			pos += float3(aiTranslation.x, aiTranslation.y, aiTranslation.z);
			scale = float3(scale.x * aiScale.x, scale.y * aiScale.y, scale.z * aiScale.z);
			rot = rot * Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

			nodeName = node->mName.C_Str();
			dummyFound = true;
		}
	}

	ModelNode newNode(randomID.Int(), nodeName.c_str(), pos, scale, rot, parentID);

	// Loading node meshes ----------------------------------------
	for (uint i = 0; i < node->mNumMeshes && i < 1; i++)
	{
		//TODO: Warning: some nodes might have 2 meshes!
		const aiMesh* newMesh = scene->mMeshes[node->mMeshes[i]];

		newNode.meshID = node->mMeshes[i];
		newNode.materialID = newMesh->mMaterialIndex;
	}

	model->nodes.push_back(newNode);

	// Load all children from the current node. As we are storing all nodes in reverse order due to recursion,
	// we will be doing the same for all the children in the same node
	for (uint i = node->mNumChildren; i > 0u; --i)
	{
		ImportNodeData(scene, node->mChildren[i - 1], model, newNode.ID);
	}
}

void Importer::Models::LinkModelResources(R_Model* model, const std::vector<uint64>& meshes, const std::vector<uint64>& materials)
{
	for (uint i = 0u; i < model->nodes.size(); ++i)
	{
		model->nodes[i].meshID = (model->nodes[i].meshID != -1 ? meshes[model->nodes[i].meshID] : 0);
		model->nodes[i].materialID = (model->nodes[i].materialID != -1 ? materials[model->nodes[i].materialID] : 0);
	}
}

uint64 Importer::Models::Save(const R_Model* model, char** buffer)
{
	Config file;
	Config_Array nodesArray = file.SetArray("Nodes");
	
	for (uint i = 0; i < model->nodes.size(); ++i)
	{
		Private::SaveModelNode(nodesArray.AddNode(), model->nodes[i]);
	}

	uint size = file.Serialize(buffer);
	return size;
}

void Importer::Models::Private::SaveModelNode(Config& config, const ModelNode& node)
{
	config.SetNumber("Node ID", node.ID);
	config.SetString("Name", node.name.c_str());

	config.SetNumber("Parent Node ID", node.parentID);

	Config_Array transformArray = config.SetArray("Transform");
	for (uint i = 0u; i < 16u; ++i)
	{
		transformArray.AddNumber(node.transform.ptr()[i]);
	}

	config.SetNumber("Mesh ID", node.meshID);
	config.SetNumber("Material ID", node.materialID);
}

void Importer::Models::Load(const char* buffer, R_Model* model)
{
	Config file(buffer);
	Config_Array nodesArray = file.GetArray("Nodes");

	for (uint i = 0; i < nodesArray.GetSize(); ++i)
	{
		Config node = nodesArray.GetNode(i);
		model->nodes.push_back(ModelNode());
		ModelNode& modelNode = (model->nodes.back());

		modelNode.ID = node.GetNumber("Node ID");
		modelNode.name = node.GetString("Name");

		modelNode.parentID = node.GetNumber("Parent Node ID");

		Config_Array transformArray = node.GetArray("Transform");
		for (uint i = 0u; i < 16; ++i)
		{
			modelNode.transform.ptr()[i] = transformArray.GetNumber(i);
		}

		modelNode.meshID = node.GetNumber("Mesh ID");
		modelNode.materialID = node.GetNumber("Material ID");
	}
}

//Process an json model buffer and loads all the GameObject hierarchy
GameObject* Importer::Models::LoadNewRoot(const R_Model* model)
{
	//We create an empty GameObject that will hold all the model data and will be removed later
	GameObject* root = nullptr;
	std::map<uint64, GameObject*> createdGameObjects;

	for (uint i = 0u; i < model->nodes.size(); ++i)
	{
		const ModelNode& node = model->nodes[i];

		//Finding the proper parent for the new GameObject
		GameObject* parent = nullptr;
		std::map<uint64, GameObject*>::iterator it = createdGameObjects.find(node.parentID);
		if (it != createdGameObjects.end())
			parent = it->second;
		
		GameObject* newGameObject = new GameObject(parent, node.transform, node.name.c_str());
		newGameObject->uid = randomID.Int(); //Warning: Do not confuse with Node IDs. Node IDs are ONLY for internal node relationships	
		createdGameObjects[node.ID] = newGameObject; //Here we store Node ID as we only use it for building parentships
		if (!parent) root = newGameObject;

		//Adding mesh component and assigning its resource (if any)
		if (node.meshID != 0)
		{
			C_Mesh* meshComponent = (C_Mesh*)newGameObject->CreateComponent(Component::Type::Mesh);
			meshComponent->SetResource(node.meshID);
		}

		//Adding material component and assigning its resource (if any)
		if (node.materialID != 0)
		{
			C_Material* materialComponent = (C_Material*)newGameObject->CreateComponent(Component::Type::Material);
			materialComponent->SetResource(node.materialID);
		}
	}

	return root;
}

R_Map* Importer::Maps::Create()
{
	return new R_Map();
}

uint64 Importer::Maps::Save(const R_Map* map, char** buffer)
{
	uint size = map->config.Serialize(buffer);
	return size;
}

void Importer::Maps::Load(const char* buffer, R_Map* scene)
{
	scene->config.Set(buffer);
	int k = 3;
}

void Importer::Maps::SaveRootToMap(GameObject* root, R_Map* map)
{
	Config_Array goArray = map->config.SetArray("GameObjects");

	std::vector<GameObject*> gameObjects;
	root->CollectChilds(gameObjects);
	gameObjects.erase(gameObjects.begin());

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		Private::SaveGameObject(goArray.AddNode(), gameObjects[i]);
	}
}

void Importer::Maps::Private::SaveGameObject(Config& config, GameObject* gameObject)
{
	gameObject->Serialize(config);
}

GameObject* Importer::Maps::LoadRootFromMap(const R_Map* scene)
{
	GameObject* root = new GameObject();
	std::map<uint64, GameObject*> createdGameObjects;
	Config_Array gameObjects_array = scene->config.GetArray("GameObjects");

	for (uint i = 0; i < gameObjects_array.GetSize(); ++i)
	{
		//Single GameObject load
		Config gameObject_node = gameObjects_array.GetNode(i);

		//Parent setup
		GameObject* parent = nullptr;
		std::map<uint64, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		GameObject* gameObject = new GameObject(parent ? parent : root);//, gameObject_node.GetString("Name").c_str(), position, rotation, scale);
		gameObject->Serialize(gameObject_node);
		createdGameObjects[gameObject->uid] = gameObject;
	}

	return root;
}