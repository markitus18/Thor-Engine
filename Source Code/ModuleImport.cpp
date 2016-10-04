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

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}
update_status ModuleImport::Update(float dt)
{
	return UPDATE_CONTINUE;
}

GameObject* ModuleImport::LoadFBX(char* path)
{
	const aiScene* file = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (file != NULL && file->HasMeshes())
	{
		// Loading meshes data
		Mesh* mesh = new Mesh;
		for (int i = 0; i < file->mNumMeshes; i++)
		{
			aiMesh* newMesh = file->mMeshes[i];
			mesh->num_vertices = newMesh->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * 3];
			memcpy(mesh->vertices, newMesh->mVertices, sizeof(float) * mesh->num_vertices * 3);
			LOG("New mesh with %d vertices", mesh->num_vertices);

			//Loading mesh faces
			if (newMesh->HasFaces())
			{
				mesh->num_indices = newMesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices];
				for (uint i = 0; i < newMesh->mNumFaces; i++)
				{
					if (newMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
						memcpy(&mesh->indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			//Loading mesh normals
			if (newMesh->HasNormals())
			{
				mesh->num_normals = newMesh->mNumVertices;
				mesh->normals = new float[mesh->num_normals * 3];
				memcpy(mesh->normals, newMesh->mNormals, sizeof(float) * mesh->num_normals * 3);
			}

			//Loading mesh texture coordinates
			if (newMesh->HasTextureCoords(0))
			{
				mesh->num_tex_coords = mesh->num_vertices;
				mesh->tex_coords = new float[newMesh->mNumVertices * 2];

				for (unsigned int i = 0; i < mesh->num_tex_coords; i++)
				{
					mesh->tex_coords[i * 2] = newMesh->mTextureCoords[0][i].x;
					mesh->tex_coords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
				}
			}
		}
		aiReleaseImport(file);

		mesh->LoadBuffers();
		GameObject* gameObject = new GameObject(*mesh);
		return gameObject;
	}
	else
	{
		LOG("Error loading file %s", path);
		return NULL;
	}

}

GLuint ModuleImport::LoadIMG(char* path)
{
	GLuint ret;
	ret = ilutGLLoadImage(path);
	return ret;
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	LOG("");

	return true;
}
