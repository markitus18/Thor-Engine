
#include "Application.h"
#include "ModuleImport.h"
#include "ModuleScene.h"

#include "C_Material.h"
#include "GameObject.h"

#include "ModuleMaterials.h"
#include "ModuleMeshes.h"
#include "ModuleFileSystem.h"

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
#include "ModuleInput.h"

ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module("Importer", start_enabled)
{}

// Destructor
ModuleImport::~ModuleImport()
{}

// Called before render is available
bool ModuleImport::Init(Config& config)
{
	LOG("Loading Module Import");

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	//ImportFile("Models/Street_environment_V01.FBX");
	//const aiScene* file2 = aiImportFile("Game/Models/3D Models/maya tmp test.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	//LoadFBX(file2, file2->mRootNode, App->scene->getRoot(), "Game/Models/3D Models/maya tmp test.fbx");

	return true;
}
update_status ModuleImport::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		ImportFile("Models/Street_environment_V01.FBX");
	}

	return UPDATE_CONTINUE;
}

void ModuleImport::ImportFile(char* path)
{
	LOG("Entering mesh load");
	const aiScene* file = aiImportFileEx(path, aiProcessPreset_TargetRealtime_MaxQuality, App->fileSystem->GetAssimpIO());
	if (file)
	{
		LOG("Starting scene load %s", path);
		LoadFBX(file, file->mRootNode, App->scene->GetRoot(), path);
		aiReleaseImport(file);
	}
	else
	{
		LOG("File not found");
	}
}

GameObject* ModuleImport::LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path)
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
		GameObject* new_child = LoadFBX(scene, node->mChildren[i], gameObject, path);
	}
	return gameObject;
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	return true;
}
