#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	uint	image_texture;
	uint	array_cube_id;
	uint	index_cube_index_id;
	uint	index_cube_vertex_id;

	float cube_vertices[24];
	uint cube_indices[36];

	uint	texture_vertex_id;
	uint	texture_UV_id;
	uint	texture_index_id;

	float	texture_vertices[16 * 3];
	float	texture_UV[16 * 2];
	uint	texture_indices[36];

	bool BuffersON = false;
};