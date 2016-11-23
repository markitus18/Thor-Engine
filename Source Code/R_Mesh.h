#ifndef __R_MESH__
#define __R_MESH__

#include "Resource.h"
#include "Globals.h"

class R_Mesh : public Resource
{
public:
	R_Mesh();
	~R_Mesh();

public:
	//Vertices data
	uint	id_vertices = 0;
	uint	num_vertices = 0;
	uint*	indices = nullptr;

	//Indices data
	uint	id_indices = 0;
	uint	num_indices = 0;
	float*	vertices = nullptr;

	//Normals data
	uint	id_normals = 0;
	uint	num_normals = 0;
	float*	normals = nullptr;

	//Texture coords
	uint	id_tex_coords = 0;
	uint	num_tex_coords = 0;
	float*	tex_coords = nullptr;
};

//hashet string


#endif // !__R_MESH__

