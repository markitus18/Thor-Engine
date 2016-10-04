#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"

class Mesh
{
public:
	Mesh();
	~Mesh();
	void LoadData(char* path);
	void LoadBuffers();
	void Draw();
public:
	//Vertices data
	uint	id_vertices = 0;
	uint	num_vertices = 0;
	uint*	indices = NULL;

	//Indices data
	uint	id_indices = 0;
	uint	num_indices = 0;
	float*	vertices = NULL;

	//Normals data
	uint	id_normals = 0;
	uint	num_normals = 0;
	float*	normals = NULL;

	//UV's data
	uint	id_UV = 0;
	uint	num_UV = 0;
	float*	UV_coords = NULL;
};

#endif

