#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "ModuleScene.h"

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



ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleImport::~ModuleImport()
{}

// Called before render is available
bool ModuleImport::Init()
{
	LOG("Loading Module Import");

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}
update_status ModuleImport::Update(float dt)
{
	if (!fbx_loaded)
	{
		fbx_loaded = true;
		const aiScene* file = aiImportFile("Game/maya tmp test.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
		LoadFBX(file, file->mRootNode, App->scene->getRoot(), "Game / maya tmp test.fbx");

	}
	return UPDATE_CONTINUE;
}

GameObject* ModuleImport::LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path)
{
	//TODO: get all transforms
	aiVector3D		translation;
	aiVector3D		scaling;
	aiQuaternion	rotation;
	
	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(scaling, rotation, translation);
	//node->mTransformation.
	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	
	//TODO: skipp all dummy modules. Assimp loads fbx nodes to stack all transformations
	std::string name = node->mName.C_Str();	
	
	for (int i = 0; i < 5; ++i)
	{
		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (name.find("$AssimpFbx$") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next GameObject
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(scaling, rotation, translation);
			pos += float3(translation.x, translation.y, translation.z);
			scale = float3(scale.x * scaling.x, scale.y * scaling.y, scale.z * scaling.z);
			rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			name = node->mName.C_Str();

			//if we find a dummy node we "change" our current node into the dummy one and search
			//for other dummy nodes inside that one.
			i = -1;
		}
	}
	
	//Cutting path into file name --------------------
	if (name == "RootNode")
	{
		name = path;
		uint slashPos;
		if ((slashPos = name.find_last_of("/")) != std::string::npos)
			name = name.substr(slashPos + 2, name.size() - slashPos);

		uint pointPos;
		if ((pointPos = name.find_first_of(".")) != std::string::npos)
			name = name.substr(0, name.size() - (name.size() - pointPos));
	}
	//-----------------------------------------------

	GameObject* gameObject = new GameObject(parent, pos, scale, rot, name.c_str());
	parent->childs.push_back(gameObject);
	App->scene->tmp_goCount++;

	// Loading node meshes ----------------------------------------
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[node->mMeshes[i]];
		GameObject* child = NULL;

		if (node->mNumMeshes > 1)
		{
			name = newMesh->mName.C_Str();
			GameObject* child = new GameObject(parent, name.c_str());
			App->scene->tmp_goCount++;
			gameObject->childs.push_back(child);
		}
		else
		{
			child = gameObject;
		}

		Mesh* go_mesh = new Mesh();

		//Loading mesh vertices data
		go_mesh->num_vertices = newMesh->mNumVertices;
		go_mesh->vertices = new float[go_mesh->num_vertices * 3];
		memcpy(go_mesh->vertices, newMesh->mVertices, sizeof(float) * go_mesh->num_vertices * 3);
		LOG("New mesh with %d vertices", go_mesh->num_vertices);

		//Loading mesh faces data
		if (newMesh->HasFaces())
		{
			go_mesh->num_indices = newMesh->mNumFaces * 3;
			go_mesh->indices = new uint[go_mesh->num_indices];
			for (uint i = 0; i < newMesh->mNumFaces; i++)
			{
				if (newMesh->mFaces[i].mNumIndices != 3)
				{
					LOG("WARNING, geometry face with != 3 indices!");
				}
				else
				{
					//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
					memcpy(&go_mesh->indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}
		}

		//Loading mesh normals data ------------------
		if (newMesh->HasNormals())
		{
			go_mesh->num_normals = newMesh->mNumVertices;
			go_mesh->normals = new float[go_mesh->num_normals * 3];
			memcpy(go_mesh->normals, newMesh->mNormals, sizeof(float) * go_mesh->num_normals * 3);
		}

		//Loading mesh texture coordinates -----------
		if (newMesh->HasTextureCoords(0))
		{
			go_mesh->num_tex_coords = go_mesh->num_vertices;
			go_mesh->tex_coords = new float[newMesh->mNumVertices * 2];

			for (unsigned int i = 0; i < go_mesh->num_tex_coords; i++)
			{
				go_mesh->tex_coords[i * 2] = newMesh->mTextureCoords[0][i].x;
				go_mesh->tex_coords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
			}
		}
		//-------------------------------------------

		go_mesh->LoadBuffers();
		child->AddMesh(go_mesh);
	}
	// ------------------------------------------------------------
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		GameObject* new_child = LoadFBX(scene, node->mChildren[i], gameObject, path);
		App->scene->tmp_goCount++;
		//if (new_child)
		//	gameObject->childs.push_back(new_child);
	}

	//GameObject* gameObject = new GameObject(*mesh);
	return gameObject;
}

uint ModuleImport::LoadIMG(char* path)
{
	uint ret;
	ret = ilutGLLoadImage(path);
	return ret;
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	LOG("");

	return true;
}
