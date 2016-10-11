#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "Component.h"

#include <vector>

class GameObject;

class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* new_GameObject);
	~C_Mesh();

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
	uint	id_flipped_normals;
	uint	num_normals = 0;
	float*	normals = NULL;
	float*	flipped_normals = NULL;

	//Texture coords
	uint	id_tex_coords = 0;
	uint	num_tex_coords = 0;
	float*	tex_coords = NULL;

};

#endif

