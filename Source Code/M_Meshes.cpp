#include "M_Meshes.h"

#include "C_Mesh.h"
#include "R_Mesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

R_Mesh* Importer::Meshes::Import(const aiMesh* mesh, R_Mesh* resMesh)
{
	//Loading mesh vertices data
	resMesh->buffersSize[R_Mesh::b_vertices] = mesh->mNumVertices;
	resMesh->vertices = new float[resMesh->buffersSize[R_Mesh::b_vertices] * 3];
	memcpy(resMesh->vertices, mesh->mVertices, sizeof(float) * resMesh->buffersSize[R_Mesh::b_vertices] * 3);

	LOG("New mesh with %d vertices", resMesh->buffersSize[R_Mesh::b_vertices]);

	//Loading mesh faces data
	if (mesh->HasFaces())
	{
		resMesh->buffersSize[R_Mesh::b_indices] = mesh->mNumFaces * 3;
		resMesh->indices = new uint[resMesh->buffersSize[R_Mesh::b_indices]];
		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("[warning], geometry face with != 3 indices!");
			}
			else
			{
				//Copying each face, we skip 3 slots in indices because an aiFace is made of 3 uints
				memcpy(&resMesh->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}

	//Loading mesh normals data ------------------
	if (mesh->HasNormals())
	{
		resMesh->buffersSize[R_Mesh::b_normals] = mesh->mNumVertices;
		resMesh->normals = new float[resMesh->buffersSize[R_Mesh::b_normals] * 3];
		memcpy(resMesh->normals, mesh->mNormals, sizeof(float) * resMesh->buffersSize[R_Mesh::b_normals] * 3);
	}

	//Loading mesh texture coordinates -----------
	if (mesh->HasTextureCoords(0))
	{
		resMesh->buffersSize[R_Mesh::b_tex_coords] = mesh->mNumVertices;
		resMesh->tex_coords = new float[mesh->mNumVertices * 2];

		for (unsigned int i = 0; i < resMesh->buffersSize[R_Mesh::b_tex_coords]; i++)
		{
			resMesh->tex_coords[i * 2] = mesh->mTextureCoords[0][i].x;
			resMesh->tex_coords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
	}

	resMesh->CreateAABB();
	return resMesh;
}

uint64 Importer::Meshes::Save(const R_Mesh* mesh, char** buffer)
{
	//Buffers sizes + index buffer + vertex buffer + normal buffer + texture coord buffer
	uint size = sizeof(mesh->buffersSize) + (sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices]) + (sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3)
			 + (sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3) + (sizeof(float) * mesh->buffersSize[R_Mesh::b_tex_coords] * 2);

	// Allocate buffer size
	*buffer = new char[size];
	char* cursor = *buffer;

	// First store ranges
	uint bytes = sizeof(mesh->buffersSize);
	memcpy(cursor, mesh->buffersSize, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices];
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	//Store normals - TODO: buffersSize is always 4, so ranges for this buffer will be saved
	if (mesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3;
		memcpy(cursor, mesh->normals, bytes);
		cursor += bytes;
	}

	//Store texture coordinates - TODO: buffersSize is always 4, so ranges for this buffer will be saved
	if (mesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 2;
		memcpy(cursor, mesh->tex_coords, bytes);
		cursor += bytes;
	}

	//TODO: Store mesh AABB

	return size;
}

void Importer::Meshes::Load(const char* buffer, R_Mesh* mesh)
{
	const char* cursor = buffer;

	uint bytes = sizeof(mesh->buffersSize);
	memcpy(mesh->buffersSize, cursor, bytes);
	cursor += bytes;

	//Code breaks here due to huge ranges value
	bytes = sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices];
	mesh->indices = new uint[mesh->buffersSize[R_Mesh::b_indices]];
	memcpy(mesh->indices, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3;
	mesh->vertices = new float[mesh->buffersSize[R_Mesh::b_vertices] * 3];
	memcpy(mesh->vertices, cursor, bytes);
	cursor += bytes;

	if (mesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3;
		mesh->normals = new float[mesh->buffersSize[R_Mesh::b_normals] * 3];
		memcpy(mesh->normals, cursor, bytes);
		cursor += bytes;
	}

	if (mesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_tex_coords] * 2;
		mesh->tex_coords = new float[mesh->buffersSize[R_Mesh::b_tex_coords] * 2];
		memcpy(mesh->tex_coords, cursor, bytes);
		cursor += bytes;
	}

	mesh->CreateAABB();
	mesh->LoadOnMemory();	//TODO: Do we need to load buffers here?
}