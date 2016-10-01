#include "Globals.h"
#include "Application.h"
#include "ModuleImport.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleImport::~ModuleImport()
{}

// Called before render is available
bool ModuleImport::Init()
{
	LOG("Loading Module Import");

	//Assimp initialization -------------
	LoadFBX("Game/warrior.FBX");

	return true;
}
update_status ModuleImport::Update(float dt)
{
	Mesh* mesh = &robotMesh;

	return UPDATE_CONTINUE;
}

Mesh* ModuleImport::GetMesh()
{
	return &robotMesh;
}

void ModuleImport::LoadFBX(char* path)
{
	const aiScene* file = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (file != nullptr && file->HasMeshes())
	{
		// Loading meshes data
		for (int i = 0; i < file->mNumMeshes; i++)
		{
			aiMesh* newMesh = file->mMeshes[i];
			robotMesh.num_vertices = newMesh->mNumVertices;
			robotMesh.vertices = new float[robotMesh.num_vertices * 3];
			memcpy(robotMesh.vertices, newMesh->mVertices, sizeof(float) * robotMesh.num_vertices * 3);
			LOG("New mesh with %d vertices", robotMesh.num_vertices);

			//Loading mesh faces
			if (file->mMeshes[i]->HasFaces())
			{
				robotMesh.num_indices = newMesh->mNumFaces * 3;
				robotMesh.indices = new uint[robotMesh.num_indices];

				for (uint i = 0; i < newMesh->mNumFaces; i++)
				{
					if (newMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
						memcpy(&robotMesh.indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
		}

		aiReleaseImport(file);
	}
	else
		LOG("Error loading file %s", path);
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	LOG("");

	return true;
}
