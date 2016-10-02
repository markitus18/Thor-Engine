#ifndef __MESH_H__
#define __MESH_H__

#include "Glew\"
class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	uint	id_vertices = 0;
	uint	num_vertices = 0;
	uint*	indices = NULL;

	uint	id_indices = 0;
	uint	num_indices = 0;
	float*	vertices = NULL;

	uint	id_normals = 0;
	uint	num_normals = 0;
	float*	normals = NULL;
};

#endif

