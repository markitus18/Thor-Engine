#include "C_Mesh.h"

#include "Engine.h"
#include "M_Resources.h"

#include "GameObject.h"

#include "R_Mesh.h"

#include "C_Transform.h"

#include "OpenGL.h"

C_Mesh::C_Mesh() : Component(Type::Mesh, nullptr, true)
{
}

C_Mesh::C_Mesh(GameObject* new_GameObject) : Component(Type::Mesh, new_GameObject, true)
{
}

C_Mesh::~C_Mesh()
{

}

const AABB& C_Mesh::GetAABB() const
{
	if (resourceID != 0)
		return ((R_Mesh*)Engine->moduleResources->GetResource(resourceID))->aabb;
	else
		return AABB(float3(-.5f, -.5f, .5f), float3(.5f, .5f, .5f));
}

void C_Mesh::StartBoneDeformation()
{
	bool newMesh = false; 
	if (animMesh == nullptr)
	{
		newMesh = true;
		animMesh = new R_Mesh();
		R_Mesh* rMesh = (R_Mesh*)GetResource();
		animMesh->buffersSize[R_Mesh::b_vertices] = rMesh->buffersSize[R_Mesh::b_vertices];
		animMesh->buffersSize[R_Mesh::b_normals] = rMesh->buffersSize[R_Mesh::b_normals];
		animMesh->buffersSize[R_Mesh::b_indices] = rMesh->buffersSize[R_Mesh::b_indices];
		animMesh->buffersSize[R_Mesh::b_tex_coords] = rMesh->buffersSize[R_Mesh::b_tex_coords];

		animMesh->vertices = new float[rMesh->buffersSize[R_Mesh::b_vertices] * 3];
		animMesh->normals = new float[rMesh->buffersSize[R_Mesh::b_normals] * 3];
		animMesh->indices = new uint [rMesh->buffersSize[R_Mesh::b_indices]];
		animMesh->tex_coords = new float[rMesh->buffersSize[R_Mesh::b_tex_coords] * 2];

		memcpy(animMesh->indices, rMesh->indices, rMesh->buffersSize[R_Mesh::b_indices] * sizeof(uint));
		memcpy(animMesh->tex_coords, rMesh->tex_coords, rMesh->buffersSize[R_Mesh::b_tex_coords] * 2 * sizeof(float));
	}

	memset(animMesh->vertices, 0, animMesh->buffersSize[R_Mesh::b_vertices] * sizeof(float) * 3);

	if (animMesh->buffersSize[R_Mesh::b_normals] > 0)
	{
		memset(animMesh->normals, 0, animMesh->buffersSize[R_Mesh::b_normals] * sizeof(float) * 3);
	}
	if (animMesh->buffersSize[R_Mesh::b_tex_coords] > 0)
	{
		memset(animMesh->normals, 0, animMesh->buffersSize[R_Mesh::b_tex_coords] * sizeof(float) * 2);
	}

	if (newMesh)
		animMesh->LoadSkinnedBuffers(true);
}

void C_Mesh::DeformAnimMesh()
{
	//Just for security
	if (animMesh == nullptr)
		StartBoneDeformation();

	R_Mesh* rMesh = (R_Mesh*)GetResource();
		
	std::map<std::string, GameObject*> boneMapping;
	GetBoneMapping(boneMapping);

	std::vector<float4x4> boneTransforms;
	boneTransforms.resize(rMesh->boneOffsets.size());
	std::map<std::string, uint>::iterator it;

	for (it = rMesh->boneMapping.begin(); it != rMesh->boneMapping.end(); ++it)
	{
		GameObject* bone = boneMapping[it->first];

		//TODO: Here we are just picking bone global transform, we need the bone transform matrix
		float4x4 mat = rootBone->parent->parent->GetComponent<C_Transform>()->GetGlobalTransform().Inverted();
		mat = mat * bone->GetComponent<C_Transform>()->GetGlobalTransform();
		mat = gameObject->GetComponent<C_Transform>()->GetTransform().Inverted() * mat;

		mat = mat * rMesh->boneOffsets[it->second];
		boneTransforms[it->second] = mat;
	}

	//Iterate all mesh vertex
	for (uint v = 0; v < rMesh->buffersSize[R_Mesh::b_vertices]; ++v)
	{
		float3 originalV(&rMesh->vertices[v * 3]);
			
		//Iterate all 4 bones for that vertex
		for (uint b = 0; b < 4; ++b)
		{
			int boneID = rMesh->boneIDs[v * 4 + b];
			float boneWeight = rMesh->boneWeights[v * 4 + b];

			if (boneID == -1) continue;
				
			//Transforming original mesh vertex with bone transformation matrix
			float3 toAdd = boneTransforms[boneID].TransformPos(float3(&rMesh->vertices[v * 3]));

			animMesh->vertices[v * 3] += toAdd.x  * boneWeight;
			animMesh->vertices[v * 3 + 1] += toAdd.y * boneWeight;
			animMesh->vertices[v * 3 + 2] += toAdd.z * boneWeight;

			if (rMesh->buffersSize[R_Mesh::b_normals] > 0)
			{
				//Transforming original mesh normal with bone transformation matrix
				toAdd = boneTransforms[boneID].TransformPos(float3(&rMesh->normals[v * 3]));
				animMesh->normals[v * 3] += toAdd.x * boneWeight;
				animMesh->normals[v * 3 + 1] += toAdd.y * boneWeight;
				animMesh->normals[v * 3 + 2] += toAdd.z * boneWeight;
			}
		}
	}

	animMesh->LoadSkinnedBuffers();
}

void C_Mesh::GetBoneMapping(std::map<std::string, GameObject*>& boneMapping)
{
	boneMapping.clear();
	std::vector<GameObject*> gameObjects;
	rootBone->CollectChilds(gameObjects);

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		boneMapping[gameObjects[i]->name] = gameObjects[i];
	}
}