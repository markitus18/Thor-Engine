#include "I_Scenes.h"

//TODO: Temporal engine include. Workaround until scene owns resources
#include "Engine.h"
#include "M_Resources.h"

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
#include "R_Scene.h"

#include "Config.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "MathGeoLib\src\MathGeoLib.h"

//TODO: kind of a dirty method to have a private variable in the namespace
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
	newNode.ID = randomID.Int();

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
		//TODO: We need to check what happens when we import a node that has no mesh or material. ID = -1?
		model->nodes[i].meshID = meshes[model->nodes[i].meshID];
		model->nodes[i].materialID = materials[model->nodes[i].materialID];
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

	//We create an empty GameObject that will hold all the model data and will be removed later
	model->root = new GameObject();
	std::map<uint64, GameObject*> createdGameObjects;

	for (uint i = 0u; i < nodesArray.GetSize(); ++i)
	{
		Config modelNode = nodesArray.GetNode(i);

		//Finding the proper parent for the new GameObject
		GameObject* parent = model->root;
		std::map<uint64, GameObject*>::iterator it = createdGameObjects.find(modelNode.GetNumber("Parent Node ID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		//Gathering all transform data
		float4x4 transform;
		Config_Array transformArray = modelNode.GetArray("Transform");
		for (uint i = 0u; i < 16; ++i)
		{
			transform.ptr()[i] = transformArray.GetNumber(i);
		}

		GameObject* newGameObject = new GameObject(parent, transform, modelNode.GetString("Name").c_str());
		newGameObject->uid = randomID.Int(); //Warning: Do not confuse with Node IDs. Node IDs are ONLY for internal node relationships	
		createdGameObjects[modelNode.GetNumber("Node ID")] = newGameObject; //Here we store Node ID as we only use it for building parentships

		//Adding mesh component and assignint its resource (if any)
		uint64 meshID = 0;
		if ((meshID = modelNode.GetNumber("Mesh ID")) != 0)
		{
			C_Mesh* meshComponent = (C_Mesh*)newGameObject->CreateComponent(Component::Type::Mesh);
			meshComponent->SetResource(meshID);
		}

		//Adding material component and assignint its resource (if any)
		uint64 materialID = 0;
		if ((materialID = modelNode.GetNumber("Material ID")) != 0)
		{
			C_Material* materialComponent = (C_Material*)newGameObject->CreateComponent(Component::Type::Material);
			materialComponent->SetResource(materialID);
		}
	}
}

R_Scene* Importer::Scenes::Create()
{
	return new R_Scene();
}

uint64 Importer::Scenes::Save(const R_Scene* scene, char** buffer)
{
	Config file;
	Config_Array goArray = file.SetArray("GameObjects");

	std::vector<const GameObject*> gameObjects;
	scene->root->CollectChilds(gameObjects);
	gameObjects.erase(gameObjects.begin());

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		Private::SaveGameObject(goArray.AddNode(), gameObjects[i]);
	}

	uint size = file.Serialize(buffer);
	return size;
}

void Importer::Scenes::Private::SaveGameObject(Config& config, const GameObject* gameObject)
{
	config.SetNumber("UID", gameObject->uid);

	config.SetNumber("ParentUID", gameObject->parent ? gameObject->parent->uid : 0);
	config.SetString("Name", gameObject->name.c_str());

	config.SetBool("Active", gameObject->active);
	config.SetBool("Static", gameObject->isStatic);
	config.SetBool("Selected", gameObject->IsSelected());
	config.SetBool("OpenInHierarchy", gameObject->hierarchyOpen);

	const C_Transform* transform = gameObject->GetComponent<C_Transform>();

	//Translation part
	config.SetArray("Translation").AddFloat3(transform->GetPosition());

	//Rotation part
	config.SetArray("Rotation").AddQuat(transform->GetQuatRotation());

	//Scale part
	config.SetArray("Scale").AddFloat3(transform->GetScale());

	Config_Array compConfig = config.SetArray("Components");
	const std::vector<Component*> components = gameObject->GetAllComponents();

	for (uint i = 0; i < components.size(); i++)
	{
		SaveComponentBase(compConfig.AddNode(), components[i]);
	}
}

void Importer::Scenes::Private::SaveComponentBase(Config& config, const Component* component)
{
	config.SetNumber("ComponentType", (int)component->GetType());
	Private::SaveComponent(config, component);

	config.SetBool("HasResource", component->HasResource());
	if (component->HasResource())
	{
		config.SetNumber("ID", component->GetResourceID());
	}
}

void Importer::Scenes::Private::SaveComponent(Config& config, const Component* component)
{
	switch (component->GetType())
	{
		case(Component::Camera):
			Private::SaveComponent(config, (C_Camera*)component);
			break;
		case(Component::Animator):
			Private::SaveComponent(config, (C_Animator*)component);
			break;
	}
}

void Importer::Scenes::Private::SaveComponent(Config& config, const C_Camera* camera)
{
	config.SetNumber("FOV", camera->frustum.VerticalFov() * RADTODEG);
	config.SetNumber("NearPlane", camera->frustum.NearPlaneDistance());
	config.SetNumber("FarPlane", camera->frustum.FarPlaneDistance());
}

void Importer::Scenes::Private::SaveComponent(Config& config, const C_Animator* animator)
{
	config.SetBool("Playing", animator->playing);
	config.SetNumber("Current Animation", animator->current_animation);
}

//TODO: Do we need to fill this function ??
void Importer::Scenes::Private::SaveComponent(Config& config, const C_ParticleSystem* component)
{

}

void Importer::Scenes::Load(const char* buffer, R_Scene* scene)
{
	Config file(buffer);
	scene->root = new GameObject();
	std::map<uint64, GameObject*> createdGameObjects;
	Config_Array gameObjects_array = file.GetArray("GameObjects");

	for (uint i = 0; i < gameObjects_array.GetSize(); ++i)
	{
		//Single GameObject load
		Config gameObject_node = gameObjects_array.GetNode(i);

		float3 position = gameObject_node.GetArray("Translation").GetFloat3(0);
		Quat rotation = gameObject_node.GetArray("Rotation").GetQuat(0);
		float3 scale = gameObject_node.GetArray("Scale").GetFloat3(0);

		//Parent setup
		GameObject* parent = nullptr;
		std::map<uint64, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		GameObject* gameObject = new GameObject(parent ? parent : scene->root, gameObject_node.GetString("Name").c_str(), position, rotation, scale);
		gameObject->uid = gameObject_node.GetNumber("UID");
		createdGameObjects[gameObject->uid] = gameObject;

		gameObject->active = gameObject_node.GetBool("Active");
		gameObject->isStatic = gameObject_node.GetBool("Static");
		
		//if (gameObject_node.GetBool("Selected", false))
		//	Engine->moduleEditor->AddSelect(gameObject);

		gameObject->beenSelected = gameObject->hierarchyOpen = gameObject_node.GetBool("OpenInHierarchy", false);

		Config_Array components = gameObject_node.GetArray("Components");

		for (uint i = 0; i < components.GetSize(); i++)
		{
			Config comp = components.GetNode(i);
			Component::Type type = (Component::Type)((int)comp.GetNumber("ComponentType"));

			if (Component* component = gameObject->CreateComponent(type))
			{
				if (comp.GetBool("HasResource") == true)
				{
					component->SetResource(comp.GetNumber("ID"));
				}
				Private::LoadComponent(comp, component);
			}

		}

		//Call OnUpdateTransform() to init all components according to the GameObject
		gameObject->OnUpdateTransform();
	}
}

void Importer::Scenes::Private::LoadComponent(Config& config, Component* component)
{
	switch (component->GetType())
	{
	case(Component::Camera):
	{
		Private::LoadComponent(config, (C_Camera*)component);
		break;
	}
	case(Component::Animator):
	{
		Private::LoadComponent(config, (C_Animator*)component);
		break;
	}
	}
}

void Importer::Scenes::Private::LoadComponent(Config& config, C_Camera* camera)
{
	camera->SetFOV(config.GetNumber("FOV"));
	camera->SetNearPlane(config.GetNumber("NearPlane"));
	camera->SetFarPlane(config.GetNumber("FarPlane"));
}

void Importer::Scenes::Private::LoadComponent(Config& config, C_Animator* animator)
{
	animator->playing = config.GetBool("Playing");
	uint currentAnimation = config.GetNumber("Current Animation");
	animator->SetAnimation(currentAnimation);
}

void Importer::Scenes::Private::LoadComponent(Config& config, C_ParticleSystem* particleSystem)
{

}