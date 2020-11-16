#include "C_Mesh.h"

#include "Engine.h"
#include "M_Resources.h"

#include "GameObject.h"

#include "R_Mesh.h"

#include "C_Material.h"
#include "C_Transform.h"

#include "M_Renderer3D.h"
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

void C_Mesh::Draw(RenderingFlags flags)
{
	StartBoneDeformation();
	DeformAnimMesh();

	Engine->renderer3D->AddMesh(gameObject->GetComponent<C_Transform>()->GetTransformT(), this, gameObject->GetComponent<C_Material>());
}

void C_Mesh::Serialize(Config& config)
{
	Component::Serialize(config);
	if (config.isSaving)
	{
		config.SetNumber("Mesh Resource", rMeshHandle.GetID());
	}
	else
	{
		uint64 resourceID = config.GetNumber("Mesh Resource");
		SetResource(resourceID);
	}
}

const AABB& C_Mesh::GetAABB() const
{
	if (rMeshHandle.GetID() != 0)
		return rMeshHandle.Get()->aabb;
	else
		return AABB(float3(-.5f, -.5f, .5f), float3(.5f, .5f, .5f));
}

void C_Mesh::StartBoneDeformation()
{
	rootBone = gameObject->parent->childs[0]->childs[0];

	bool newMesh = false; 
	if (animMesh == nullptr)
	{
		newMesh = true;
		animMesh = new R_Mesh();
		R_Mesh* rMesh = rMeshHandle.Get();
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

	R_Mesh* rMesh = rMeshHandle.Get();
		
	std::map<std::string, GameObject*> boneMapping;
	GetBoneMapping(boneMapping);

	std::vector<float4x4> boneTransforms;
	boneTransforms.resize(rMesh->boneOffsets.size());
	std::map<std::string, uint>::iterator it;

	for (it = rMesh->boneMapping.begin(); it != rMesh->boneMapping.end(); ++it)
	{
		GameObject* bone = boneMapping[it->first];

		/* Older setup: Accessing FBX's root to get the mesh in local space. Unnecessary step!!
		float4x4 mat = rootBone->parent->parent->GetComponent<C_Transform>()->GetTransform().Inverted();
		mat = mat * bone->GetComponent<C_Transform>()->GetTransform();
		mat = gameObject->GetComponent<C_Transform>()->GetLocalTransform().Inverted() * mat; */

		float4x4 mat = bone->GetComponent<C_Transform>()->GetTransform();
		mat = gameObject->GetComponent<C_Transform>()->GetTransform().Inverted() * mat; // Converting bone transform to mesh space
		mat = mat * rMesh->boneOffsets[it->second];										// Getting Delta Matrix from T-pose to current pose
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

void C_Mesh::SetResource(Resource* resource)
{
	rMeshHandle.Set((R_Mesh*)resource);
}

void C_Mesh::SetResource(unsigned long long id)
{
	rMeshHandle.Set(id);
}

uint64 C_Mesh::GetResourceID() const
{
	return rMeshHandle.GetID();
}