#include "M_Meshes.h"
#include "Application.h"

#include "C_Mesh.h"
#include "R_Mesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "M_FileSystem.h"

M_Meshes::M_Meshes(bool start_enabled) : Module("Meshes", start_enabled)
{

}

M_Meshes::~M_Meshes()
{

}

bool M_Meshes::Init()
{
	return true;
}

bool M_Meshes::CleanUp()
{
	return true;
}

C_Mesh* M_Meshes::ImportMesh(const aiMesh* from, const char* file)
{
	C_Mesh* mesh = new C_Mesh;

	//-----------------------------------------------------------------
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
				LOG("[warning], geometry face with != 3 indices!");
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

	mesh->libFile = file;
	mesh->CreateAABB();
	mesh->LoadBuffers();

	SaveMesh(*mesh, file);

	return mesh;
}

void M_Meshes::SaveMesh(const C_Mesh& mesh, const char* path)
{
	// amount of indices / vertices / normals / texture_coords
	uint ranges[4] = { mesh.num_indices, mesh.num_vertices, mesh.num_normals, mesh.num_tex_coords };

	uint size = sizeof(ranges) + (sizeof(uint) * mesh.num_indices) + (sizeof(float) * mesh.num_vertices * 3) + (sizeof(float) * mesh.num_normals * 3) + (sizeof(float) * mesh.num_tex_coords * 2);

	// Allocate buffer size
	char* data = new char[size];
	char* cursor = data;

	uint size_after_alloc = sizeof(data);

	// First store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	uint rangesTest[4];
	memcpy(rangesTest, cursor, bytes);

	cursor += bytes;


	
	// Store indices
	bytes = sizeof(uint) * mesh.num_indices;
	memcpy(cursor, mesh.indices, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float) * mesh.num_vertices * 3;
	memcpy(cursor, mesh.vertices, bytes);
	cursor += bytes;

	//Store normals
	if (mesh.num_normals > 0)
	{
		bytes = sizeof(float) * mesh.num_normals * 3;
		memcpy(cursor, mesh.normals, bytes);
		cursor += bytes;
	}

	if (mesh.num_tex_coords > 0)
	{
		//Store texture coords
		bytes = sizeof(float) * mesh.num_tex_coords * 2;
		memcpy(cursor, mesh.tex_coords, bytes);
		cursor += bytes;
	}


	std::string full_path = ("Library/Meshes/");
	full_path.append(path);// .append(".mesh");

	uint totalSizeBefore = sizeof(data);

	App->fileSystem->Save(full_path.c_str(), data, size);

	uint totalSizeAfter = sizeof(data);

	RELEASE_ARRAY(data);
}

C_Mesh* M_Meshes::LoadMesh(const char* path)
{
	std::string full_path = "Library/Meshes/";
	full_path.append(path);// .append(".mesh");

	char* buffer;
	uint size = App->fileSystem->Load(full_path.c_str(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;

		C_Mesh* mesh = new C_Mesh();

		uint ranges[4];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes;

		mesh->num_indices = ranges[0];
		mesh->num_vertices = ranges[1];
		mesh->num_normals = ranges[2];
		mesh->num_tex_coords = ranges[3];

		//Code breaks here due to huge ranges value
		bytes = sizeof(uint) * mesh->num_indices;
		mesh->indices = new uint[mesh->num_indices];
		memcpy(mesh->indices, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float) * mesh->num_vertices * 3;
		mesh->vertices = new float[mesh->num_vertices * 3];
		memcpy(mesh->vertices, cursor, bytes);
		cursor += bytes;

		if (mesh->num_normals > 0)
		{
			bytes = sizeof(float) * mesh->num_normals * 3;
			mesh->normals = new float[mesh->num_normals * 3];
			memcpy(mesh->normals, cursor, bytes);
			cursor += bytes;
		}

		if (mesh->num_tex_coords > 0)
		{
			bytes = sizeof(float) * mesh->num_tex_coords * 2;
			mesh->tex_coords = new float[mesh->num_tex_coords * 2];
			memcpy(mesh->tex_coords, cursor, bytes);
			cursor += bytes;
		}


		mesh->CreateAABB();
		mesh->LoadBuffers();
		mesh->libFile = path;

		RELEASE_ARRAY(buffer);
		return mesh;
	}
	else
	{
		LOG("[warning] mesh file '%s' is empty", path);
		return nullptr;
	}
}

R_Mesh* M_Meshes::ImportMeshResource(const aiMesh* mesh)
{
	R_Mesh* resMesh = new R_Mesh;

	//-----------------------------------------------------------------
	//Loading mesh vertices data
	resMesh->num_vertices = mesh->mNumVertices;
	resMesh->vertices = new float[resMesh->num_vertices * 3];
	memcpy(resMesh->vertices, mesh->mVertices, sizeof(float) * resMesh->num_vertices * 3);
	LOG("New mesh with %d vertices", resMesh->num_vertices);

	//Loading mesh faces data
	if (mesh->HasFaces())
	{
		resMesh->num_indices = mesh->mNumFaces * 3;
		resMesh->indices = new uint[resMesh->num_indices];
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
		resMesh->num_normals = mesh->mNumVertices;
		resMesh->normals = new float[resMesh->num_normals * 3];
		memcpy(resMesh->normals, mesh->mNormals, sizeof(float) * resMesh->num_normals * 3);
	}

	//Loading mesh texture coordinates -----------
	if (mesh->HasTextureCoords(0))
	{
		resMesh->num_tex_coords = resMesh->num_vertices;
		resMesh->tex_coords = new float[mesh->mNumVertices * 2];

		for (unsigned int i = 0; i < resMesh->num_tex_coords; i++)
		{
			resMesh->tex_coords[i * 2] = mesh->mTextureCoords[0][i].x;
			resMesh->tex_coords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
	}

	//resMesh->libfile = file;
	//mesh->CreateAABB();
	//mesh->LoadBuffers();

	//SaveMesh(*mesh, file);

	//return mesh;
	return nullptr;
}

void M_Meshes::SaveMeshResource(const R_Mesh* mesh, const char* path)
{
	// amount of indices / vertices / normals / texture_coords
	uint ranges[4] = { mesh->num_indices, mesh->num_vertices, mesh->num_normals, mesh->num_tex_coords };

	uint size = sizeof(ranges) + (sizeof(uint) * mesh->num_indices) + (sizeof(float) * mesh->num_vertices * 3) + (sizeof(float) * mesh->num_normals * 3) + (sizeof(float) * mesh->num_tex_coords * 2);

	// Allocate buffer size
	char* data = new char[size];
	char* cursor = data;

	uint size_after_alloc = sizeof(data);

	// First store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	uint rangesTest[4];
	memcpy(rangesTest, cursor, bytes);

	cursor += bytes;



	// Store indices
	bytes = sizeof(uint) * mesh->num_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float) * mesh->num_vertices * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	//Store normals
	if (mesh->num_normals > 0)
	{
		bytes = sizeof(float) * mesh->num_normals * 3;
		memcpy(cursor, mesh->normals, bytes);
		cursor += bytes;
	}

	if (mesh->num_tex_coords > 0)
	{
		//Store texture coords
		bytes = sizeof(float) * mesh->num_tex_coords * 2;
		memcpy(cursor, mesh->tex_coords, bytes);
		cursor += bytes;
	}

	App->fileSystem->Save(path, data, size);

	RELEASE_ARRAY(data);
}