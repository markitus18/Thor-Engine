#
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "MathGeoLib\src\MathGeoLib.h"

#define MAX_LIGHTS 8

class C_Camera;
class C_Mesh;
class C_Material;

struct RenderMesh
{
	RenderMesh(float4x4 trans, C_Mesh* m, C_Material* mat, bool sh, bool wire, bool selected) : transform(trans), mesh(m), material(mat), shaded(sh), wireframe(wire), selected(selected)
	{
	}
	float4x4 transform;
	C_Mesh* mesh;
	C_Material* material;
	bool shaded;
	bool wireframe;
	bool selected

};

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
	void UpdateProjectionMatrix();

	void SetActiveCamera(C_Camera* camera);

	void AddMesh(float4x4 transform, C_Mesh* mesh, C_Material* material, bool shaded, bool wireframe, bool selected);
	void DrawMesh(const RenderMesh& mesh);

public:
	//TODO: should it be moved into window module? SDL method maybe?
	int window_width;
	int window_height;

	C_Camera* camera;
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

#pragma region Cubes
	//uint	image_texture;
	//uint	array_cube_id;
	//uint	index_cube_index_id;
	//uint	index_cube_vertex_id;

	//float cube_vertices[24];
	//uint cube_indices[36];

	//uint	texture_vertex_id;
	//uint	texture_UV_id;
	//uint	texture_index_id;

	//float	texture_vertices[16 * 3];
	//float	texture_UV[16 * 2];
	//uint	texture_indices[36];

	//uint	lenna_texture;
	//bool	lenaON = false;

	//bool BuffersON = false;
#pragma endregion

private:
	std::vector<RenderMesh> meshes;
};