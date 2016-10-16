#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "C_Material.h"
#include "ModuleMaterials.h"

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

	LOG("Entering mesh load");
	const aiScene* file = aiImportFile("Game/Models/3D_Models/Street_environment_V01.FBX", aiProcessPreset_TargetRealtime_MaxQuality);
	if (file)
	{
		LOG("File found");
		LoadFBX(file, file->mRootNode, App->scene->getRoot(), "Game/Models/3D Models/Street environment_V01.FBX");
		aiReleaseImport(file);
	}
	else
	{
		LOG("File not found");
	}
	//const aiScene* file2 = aiImportFile("Game/Models/3D Models/maya tmp test.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
	//LoadFBX(file2, file2->mRootNode, App->scene->getRoot(), "Game/Models/3D Models/maya tmp test.fbx");

	return true;
}
update_status ModuleImport::Update(float dt)
{
	//LOG("Module Import update")

	return UPDATE_CONTINUE;
}

//Tmp function, move to file system
void ModuleImport::CutPath(std::string& str)
{
	uint position = str.find_last_of("\\/");
	if (position != std::string::npos)
	{
		str = str.substr(position + 1, str.size() - position);
	}
}

std::string ModuleImport::GetFileFolder(const std::string& str)
{
	std::string ret;
	uint position = str.find_last_of("\\/");
	if (position != std::string::npos)
	{
		ret = str.substr(0, position + 1);
	}
	return ret;
}

GameObject* ModuleImport::LoadFBX(const aiScene* scene, const aiNode* node, GameObject* parent, char* path)
{
	//TODO: get all transforms
	aiVector3D		translation;
	aiVector3D		scaling;
	aiQuaternion	rotation;
	
	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	
	//Skipp all dummy modules. Assimp loads this fbx nodes to stack all transformations
	std::string name = node->mName.C_Str();	
	bool dummyFound = true;
	while (dummyFound)
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
			dummyFound = true;
		}
		else
			dummyFound = false;
	}
	
	//Cutting path into file name --------------------
	if (name == "RootNode")
	{
		name = path;
		uint slashPos;
		if ((slashPos = name.find_last_of("/")) != std::string::npos)
			name = name.substr(slashPos + 1, name.size() - slashPos);

		uint pointPos;
		if ((pointPos = name.find_first_of(".")) != std::string::npos)
			name = name.substr(0, name.size() - (name.size() - pointPos));
	}
	//-----------------------------------------------

	GameObject* gameObject = new GameObject(parent, name.c_str(), pos, scale, rot);


	// Loading node meshes ----------------------------------------

	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[node->mMeshes[i]];
		GameObject* child = NULL;

		if (node->mNumMeshes > 1)
		{
			name = newMesh->mName.C_Str();
			if (name == "")
				name = "No name";
			child = new GameObject(gameObject, name.c_str());
		}
		else
		{
			child = gameObject;
		}

		C_Mesh* mesh = (C_Mesh*)child->CreateComponent(Component::Type::Mesh);
		LoadMesh(mesh, newMesh);

		//Loading mesh materials ---------
		aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
		uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
		std::string mat_path = "";
		std::string file = "";
		if (numTextures > 0)
		{
			aiString file_path;
			aiReturn ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &file_path);

			std::string mat_path_str(GetFileFolder(path));
			mat_path_str += file_path.C_Str();
			mat_path = mat_path_str;
			std::string file_str = file_path.C_Str();
			CutPath(file_str);
			file = (char*)file_str.c_str();
		}
		aiColor4D color;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		C_Material* go_mat = App->moduleMaterials->LoadMaterial(mat_path, file, Color(color.r, color.g, color.b, color.a));
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


void ModuleImport::LoadMesh(C_Mesh* mesh, const aiMesh* from)
{
	//Loading mesh vertices data
	mesh->num_vertices = from->mNumVertices;
	mesh->vertices = new float[mesh->num_vertices * 3];
	memcpy(mesh->vertices, from->mVertices, sizeof(float) * mesh->num_vertices * 3);
	LOG("New mesh with %d vertices", mesh->num_vertices);

	//Loading mesh faces data
	if (from->HasFaces())
	{
		mesh->num_indices = from->mNumFaces * 3;
		mesh->indices = new uint[mesh->num_indices];
		for (uint i = 0; i < from->mNumFaces; i++)
		{
			if (from->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
				memcpy(&mesh->indices[i * 3], from->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}

	//Loading mesh normals data ------------------
	if (from->HasNormals())
	{
		mesh->num_normals = from->mNumVertices;
		mesh->normals = new float[mesh->num_normals * 3];
		memcpy(mesh->normals, from->mNormals, sizeof(float) * mesh->num_normals * 3);
	}

	//Loading mesh texture coordinates -----------
	if (from->HasTextureCoords(0))
	{
		mesh->num_tex_coords = mesh->num_vertices;
		mesh->tex_coords = new float[from->mNumVertices * 2];

		for (unsigned int i = 0; i < mesh->num_tex_coords; i++)
		{
			mesh->tex_coords[i * 2] = from->mTextureCoords[0][i].x;
			mesh->tex_coords[i * 2 + 1] = from->mTextureCoords[0][i].y;
		}
	}
	//-------------------------------------------

	mesh->LoadBuffers();
}

uint ModuleImport::LoadIMG(const char* path)
{
	uint ret;
	ret = ilutGLLoadImage((char*)path);
	return ret;
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	LOG("");

	return true;
}
