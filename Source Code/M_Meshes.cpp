#include "M_Meshes.h"

#include "C_Mesh.h"
#include "R_Mesh.h"

#include "Assimp/include/scene.h"

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

	//Loading bone data
	if (mesh->HasBones())
	{
		Private::ImportBones(mesh, resMesh);

	}
	resMesh->CreateAABB();
	return resMesh;
}

void Importer::Meshes::Private::ImportBones(const aiMesh* mesh, R_Mesh* rMesh)
{
	rMesh->boneTransforms.reserve(mesh->mNumBones);
	rMesh->buffersSize[R_Mesh::b_bone_IDs] = rMesh->buffersSize[R_Mesh::b_bone_weights] = mesh->mNumVertices * 4;

	rMesh->boneIDs = new int[rMesh->buffersSize[R_Mesh::b_bone_IDs]];
	for (uint i = 0; i < rMesh->buffersSize[R_Mesh::b_bone_IDs]; ++i) rMesh->boneIDs[i] = -1;

	rMesh->boneWeights = new float[rMesh->buffersSize[R_Mesh::b_bone_weights]];
	for (uint i = 0; i < rMesh->buffersSize[R_Mesh::b_bone_weights]; ++i) rMesh->boneWeights[i] = .0f;

	//Iterate all bones for that mesh
	for (uint boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		aiBone* bone = mesh->mBones[boneIndex];
		rMesh->boneMapping[bone->mName.C_Str()] = boneIndex;

		float4x4 offset = float4x4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
			bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
			bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
			bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4);
		rMesh->boneOffsets.push_back(offset);

		//Iterate all affected vertices
		for (uint i = 0; i < bone->mNumWeights; ++i)
		{
			uint index = bone->mWeights[i].mVertexId;

			//Find an empty bone slot for that vertex, fill ID and weight
			for (uint w = 0; w < 4; ++w)
			{
				if (rMesh->boneIDs[index*4 + w] == -1)
				{
					rMesh->boneIDs[index * 4 + w] = boneIndex;
					rMesh->boneWeights[index * 4 + w] = bone->mWeights[i].mWeight;
				}
			}
		}
	}
}

uint64 Importer::Meshes::Save(const R_Mesh* mesh, char** buffer)
{
	//Buffers sizes + bone offset size
	//+ index buffer + vertex buffer
	//+ normal buffer + texture coord buffer
	//+ bone IDs buffer + bone weights buffer
	//+ bone offsets buffer + bone mapping strings
	uint size = sizeof(mesh->buffersSize) + sizeof(uint)
			 + sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices] + (sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3)
			 + sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3 + (sizeof(float) * mesh->buffersSize[R_Mesh::b_tex_coords] * 2) 
		     + sizeof (int) * mesh->buffersSize[R_Mesh::b_bone_IDs] + sizeof(float) * mesh->buffersSize[R_Mesh::b_bone_weights]
			 + sizeof(float) * 16 * mesh->boneOffsets.size() + sizeof(char) * 30 * mesh->boneMapping.size();

	// Allocate buffer size
	*buffer = new char[size];
	char* cursor = *buffer;

	// First store ranges
	uint bytes = sizeof(mesh->buffersSize);
	memcpy(cursor, mesh->buffersSize, bytes);
	cursor += bytes;

	// Store bone offset size
	bytes = sizeof(uint);
	uint bonesSize = mesh->boneOffsets.size();
	memcpy(cursor, &bonesSize, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(uint) * mesh->buffersSize[R_Mesh::b_indices];
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_vertices] * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	//Store normals
	if (mesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_normals] * 3;
		memcpy(cursor, mesh->normals, bytes);
		cursor += bytes;
	}

	//Store texture coordinates
	if (mesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		bytes = sizeof(float) * mesh->buffersSize[R_Mesh::b_tex_coords] * 2;
		memcpy(cursor, mesh->tex_coords, bytes);
		cursor += bytes;
	}

	Private::SaveBones(mesh, &cursor);

	//TODO: Store mesh AABB

	return size;
}

void Importer::Meshes::Private::SaveBones(const R_Mesh* rMesh, char** cursor)
{
	//Store bone IDs
	uint bytes = 0;

	if (rMesh->buffersSize[R_Mesh::b_bone_IDs] > 0)
	{
		bytes = sizeof(int) * rMesh->buffersSize[R_Mesh::b_bone_IDs];
		memcpy(*cursor, rMesh->boneIDs, bytes);
		*cursor += bytes;
	}

	if (rMesh->buffersSize[R_Mesh::b_bone_weights] > 0)
	{
		bytes = sizeof(float) * rMesh->buffersSize[R_Mesh::b_bone_weights];
		memcpy(*cursor, rMesh->boneWeights, bytes);
		*cursor += bytes;
	}

	if (rMesh->boneOffsets.size() > 0)
	{
		for (uint i = 0; i < rMesh->boneOffsets.size(); ++i)
		{
			bytes = sizeof(float) * 16;
			memcpy(*cursor, rMesh->boneOffsets[i].ptr(), bytes);
			*cursor += bytes;
		}
	}

	//Storing all strings in order so there is no need to store the mapping
	for (uint ID = 0; ID < rMesh->boneMapping.size(); ++ID)
	{
		std::map<std::string, uint>::const_iterator it;
		
		for (it = rMesh->boneMapping.begin(); it != rMesh->boneMapping.end(); ++it)
		{
			if (it->second == ID)
			{
				std::string str;
				str.reserve(30);
				str.append(it->first);
				uint size = str.size();
				memcpy(*cursor, str.c_str(), size);
			}
		}
	}
}

void Importer::Meshes::Load(const char* buffer, R_Mesh* mesh)
{
	const char* cursor = buffer;

	uint bytes = sizeof(mesh->buffersSize);
	memcpy(mesh->buffersSize, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(uint);
	uint bonesSize = 0;
	memcpy(&bonesSize, cursor, bytes);
	cursor += bytes;
	
	mesh->boneTransforms.reserve(bonesSize);

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

	Private::LoadBones(&cursor, mesh);

	mesh->CreateAABB();
	mesh->LoadOnMemory();	//TODO: Do we need to load buffers here?
}

void Importer::Meshes::Private::LoadBones(const char** cursor, R_Mesh* rMesh)
{
	uint bytes = 0;
	if (rMesh->buffersSize[R_Mesh::b_bone_IDs] > 0)
	{
		bytes = sizeof(int) * rMesh->buffersSize[R_Mesh::b_bone_IDs];
		rMesh->boneIDs = new int[rMesh->buffersSize[R_Mesh::b_bone_IDs]];
		memcpy(rMesh->boneIDs, *cursor, bytes);
		*cursor += bytes;
	}

	if (rMesh->buffersSize[R_Mesh::b_bone_weights] > 0)
	{
		bytes = sizeof(float) * rMesh->buffersSize[R_Mesh::b_bone_weights];
		rMesh->boneWeights = new float[rMesh->buffersSize[R_Mesh::b_bone_weights]];
		memcpy(rMesh->boneWeights, *cursor, bytes);
		*cursor += bytes;
	}

	float matrix[16];
	for (uint i = 0; i < rMesh->boneTransforms.size(); ++i)
	{
		bytes = sizeof(float) * 16;
		memcpy(matrix, *cursor, bytes);
		*cursor += bytes;

		float4x4 offset;
		offset.Set(matrix);
	}

	char name[30];
	for (uint i = 0; i < rMesh->boneTransforms.size(); ++i)
	{
		bytes = 30;
		memcpy(name, *cursor, bytes);
		*cursor += bytes;

		std::string str(name);
		rMesh->boneMapping[str.c_str()] = i;
	}
}
