#ifndef __R_MESH__
#define __R_MESH__

#include "Resource.h"
#include "Globals.h"
#include "MathGeoLib\src\Geometry\AABB.h"

class R_Mesh : public Resource
{
public:

	enum Buffers
	{
		b_indices,
		b_vertices,
		b_normals,
		b_tex_coords,
		max_buffer_type, //Warning: this needs to be the last element
	};

	R_Mesh();
	~R_Mesh();

	void CreateAABB();

	void LoadOnMemory();
	void LoadOnMemory_Shader();
	void LoadOnMemory_Index();
	void FreeMemory();

public:

	uint buffers[max_buffer_type];
	uint buffersSize[max_buffer_type];

	//TODO: Temorary to start having some results
	uint VAO = 0;
	uint vertexVBO = 0;
	uint indexEBO = 0;
	uint normalsVBO = 0;
	uint texCoordVBO = 0;

	uint*	indices = nullptr;
	float*	vertices = nullptr;
	float*	normals = nullptr;
	float*	tex_coords = nullptr;

	AABB aabb;
};

//hashet string


#endif // !__R_MESH__

