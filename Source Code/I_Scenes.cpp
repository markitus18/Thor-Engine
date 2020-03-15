#include "I_Scenes.h"

//TODO: Temporal app include. Workaround until scene owns resources
#include "Application.h"
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
#include "R_Prefab.h"

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

void Importer::Scenes::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

void Importer::Scenes::SaveScene(const GameObject* root, Config& file)
{
	Config_Array goArray = file.SetArray("GameObjects");
	
	std::vector<const GameObject*> gameObjects;
	root->CollectChilds(gameObjects);

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		Private::SaveGameObject(goArray.AddNode(), gameObjects[i]);
	}
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
		//TODO: this should be avoided, forces resource load into memory
		const Resource* resource = component->GetResource();
		config.SetNumber("ID", component->GetResourceID());
		config.SetNumber("Type", resource->GetType());
		config.SetString("ResourceName", resource->GetName());
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

void Importer::Scenes::LoadScene(const Config& file, std::vector<GameObject*>& roots)
{
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

		GameObject* gameObject = new GameObject(parent, gameObject_node.GetString("Name").c_str(), position, scale, rotation);
		gameObject->uid = gameObject_node.GetNumber("UID");
		createdGameObjects[gameObject->uid] = gameObject;

		if (parent == nullptr) roots.push_back(gameObject);

		gameObject->active = gameObject_node.GetBool("Active");
		gameObject->isStatic = gameObject_node.GetBool("Static");
		
		//if (gameObject_node.GetBool("Selected", false))
		//	App->moduleEditor->AddSelect(gameObject);

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

void Importer::Scenes::SaveContainedResources(R_Prefab* prefab, Config& file)
{
	//TODO: The function is accesing App, which should not be done in an importer
	Config_Array containingResources = file.SetArray("Containing Resources");
	for (uint i = 0; i < prefab->containingResources.size(); ++i)
	{
		if (Resource* res = App->moduleResources->GetResource(prefab->containingResources[i]))
		{
			Config resNode = containingResources.AddNode();
			resNode.SetNumber("ID", res->GetID());
			resNode.SetString("Name", res->GetName());
			resNode.SetNumber("Type", res->GetType());
		}
	}
}

void Importer::Scenes::LoadContainedResources(const Config& file, R_Prefab* prefab)
{
	Config_Array containingResources = file.GetArray("Containing Resources");
	for (uint i = 0; i < containingResources.GetSize(); ++i)
	{
		prefab->containingResources.push_back(containingResources.GetNode(i).GetNumber("ID"));
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

//TODO: kind of a dirty method to have a private variable in the namespace
namespace Importer { namespace Scenes { LCG randomID; } }

const aiScene* Importer::Scenes::ProcessAssimpScene(const char* buffer, uint size)
{
	return aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
}

GameObject* Importer::Scenes::Import(const aiScene* scene, const char* name)
{
	GameObject* root = CreateGameObjectFromNode(scene, scene->mRootNode, nullptr);
	root->name = name;
	return root;
}

GameObject* Importer::Scenes::CreateGameObjectFromNode(const aiScene* scene, const aiNode* node, GameObject* parent)
{
	aiVector3D		translation;
	aiVector3D		scaling;
	aiQuaternion	rotation;

	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	//Assimp loads "dummy" modules to stack fbx transformation. Here we collapse all those transformations
	//to the first node that is not a dummy
	std::string node_name = node->mName.C_Str();
	bool dummyFound = false;
	do
	{
		dummyFound = false;
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
			dummyFound = true;
		}
	} while (dummyFound == true);

	GameObject* gameObject = new GameObject(parent, node_name.c_str(), pos, scale, rot);
	gameObject->uid = randomID.Int();

	if (parent == nullptr && scene->HasAnimations())
		gameObject->CreateComponent(Component::Animator);

	// Loading node meshes ----------------------------------------
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* newMesh = scene->mMeshes[node->mMeshes[i]];
		GameObject* child = gameObject;

		if (node->mNumMeshes > 1)
		{
			node_name = newMesh->mName.C_Str();
			if (node_name == "")
				node_name = gameObject->name + "_submesh";
			if (i > 0)
				node_name.append("(" + std::to_string(i) + ")");
			child = new GameObject(gameObject, node_name.c_str());
			child->uid = randomID.Int();
		}

		C_Mesh* cMesh = (C_Mesh*)child->CreateComponent(Component::Mesh);
		cMesh->SetResource(node->mMeshes[i], false);

		C_Material* cMaterial = (C_Material*)child->CreateComponent(Component::Material);
		cMaterial->SetResource(newMesh->mMaterialIndex, false);
	}

	// Load all children from the current node
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		CreateGameObjectFromNode(scene, node->mChildren[i], gameObject);
	}
	return gameObject;
}

void Importer::Scenes::LinkSceneResources(GameObject* gameObject, const std::vector<uint64>& meshes, const std::vector<uint64>& materials)
{
	//Link loaded mesh resource
	if (C_Mesh* mesh = gameObject->GetComponent<C_Mesh>())
	{
		mesh->SetResource(meshes[mesh->GetResourceID()], false);
	}
	//Link loaded mat resource
	if (C_Material* mat = gameObject->GetComponent<C_Material>())
	{
		mat->SetResource(materials[mat->GetResourceID()], false);
	}

	//Iterate all gameObject's children recursively
	for (uint i = 0; i < gameObject->childs.size(); ++i)
	{
		LinkSceneResources(gameObject->childs[i], meshes, materials);
	}
}