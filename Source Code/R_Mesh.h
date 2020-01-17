#ifndef __R_MESH__
#define __R_MESH__

#include "Resource.h"
#include "Globals.h"

#include <map>

#include "MathGeoLib/src/Geometry/AABB.h"
#include "MathGeoLib/src/Math/float4x4.h"

struct Bone
{
	uint numWeights = 0;
	uint* weightsIndex = nullptr;
	float* weights = nullptr;
	float4x4 offset = float4x4::identity;
};

class R_Mesh : public Resource
{
public:

	enum Buffers
	{
		b_indices,
		b_vertices,
		b_normals,
		b_tex_coords,
		b_bone_IDs,
		b_bone_weights,
		max_buffer_type, //Warning: this needs to be the last element
	};

	R_Mesh();
	~R_Mesh();

	void CreateAABB();

	void LoadOnMemory();
	void LoadSkinnedBuffers(bool init = false);

	void FreeMemory();

public:

	uint VAO = 0;
	uint buffers[max_buffer_type];
	uint buffersSize[max_buffer_type];

	uint*	indices = nullptr;
	float*	vertices = nullptr;
	float*	normals = nullptr;
	float*	tex_coords = nullptr;

	//Buffer sizes of 4 * vertices, 4 spaces for each vertex
	int* boneIDs = nullptr;
	float* boneWeights = nullptr;

	std::map<std::string, uint> boneMapping;
	std::vector<float4x4> boneTransforms;
	std::vector<float4x4> boneOffsets;

	AABB aabb;
};


#endif // !__R_MESH__

