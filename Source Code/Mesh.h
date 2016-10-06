#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include <vector>

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

	//Texture coords
	uint	id_tex_coords = 0;
	uint	num_tex_coords = 0;
	float*	tex_coords = NULL;

	std::vector<Mesh> childs;
};

#endif

