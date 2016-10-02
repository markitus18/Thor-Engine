#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"

struct Mesh
{
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

class ModuleImport : public Module
{
public:

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	void LoadFBX(char* path);

	bool Init();
	bool CleanUp();
	update_status Update(float dt);

	Mesh* GetMesh();
public:
	Mesh robotMesh;

private:

};

#endif