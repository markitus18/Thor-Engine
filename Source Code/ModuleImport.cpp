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
	LoadFBX("Game/RandomStuff.fbx");

	return true;
}
update_status ModuleImport::Update(float dt)
{
	Mesh* mesh = &robotMesh.mesh;

	return UPDATE_CONTINUE;
}

Mesh* ModuleImport::GetMesh()
{
	return &robotMesh.mesh;
}

void ModuleImport::LoadFBX(char* path)
{
	const aiScene* file = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (file != NULL && file->HasMeshes())
	{
		// Loading meshes data
		for (int i = 0; i < file->mNumMeshes; i++)
		{
			aiMesh* newMesh = file->mMeshes[i];
			robotMesh.mesh.num_vertices = newMesh->mNumVertices;
			robotMesh.mesh.vertices = new float[robotMesh.mesh.num_vertices * 3];
			memcpy(robotMesh.mesh.vertices, newMesh->mVertices, sizeof(float) * robotMesh.mesh.num_vertices * 3);
			LOG("New mesh with %d vertices", robotMesh.mesh.num_vertices);

			//Loading mesh faces
			if (newMesh->HasFaces())
			{
				robotMesh.mesh.num_indices = newMesh->mNumFaces * 3;
				robotMesh.mesh.indices = new uint[robotMesh.mesh.num_indices];
				for (uint i = 0; i < newMesh->mNumFaces; i++)
				{
					if (newMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
						memcpy(&robotMesh.mesh.indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			//Loading mesh normals
			if (newMesh->HasNormals())
			{
				robotMesh.mesh.num_normals = newMesh->mNumVertices;
				robotMesh.mesh.normals = new float[robotMesh.mesh.num_normals * 3];
				memcpy(robotMesh.mesh.normals, newMesh->mNormals, sizeof(float) * robotMesh.mesh.num_normals * 3);
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
