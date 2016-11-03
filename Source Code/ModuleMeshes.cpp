#include "ModuleMeshes.h"
#include "C_Mesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMeshes::ModuleMeshes(Application* app, bool start_enabled) : Module("Meshes", start_enabled)
{

}

ModuleMeshes::~ModuleMeshes()
{

}

bool ModuleMeshes::Init()
{
	return true;
}

bool ModuleMeshes::CleanUp()
{
	return true;
}

C_Mesh* ModuleMeshes::LoadMesh(const aiMesh* from)
{
	C_Mesh* mesh = new C_Mesh;

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
	mesh->CreateAABB();
	mesh->LoadBuffers();

	return mesh;
}

void ModuleMeshes::LoadBuffers(C_Mesh* mesh)
{

}

