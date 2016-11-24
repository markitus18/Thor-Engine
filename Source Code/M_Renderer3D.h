#
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "MathGeoLib\src\MathGeoLib.h"

#define MAX_LIGHTS 8

class C_Camera;
class C_Mesh;
class C_Material;

class R_Mesh;

class Config;

struct RenderMesh
{
	RenderMesh(float4x4 trans, const C_Mesh* m, const C_Material* mat, bool sh, bool wire, bool selected, bool parentSelected, bool flippedNormals) : transform(trans), mesh(m), material(mat),
		shaded(sh), wireframe(wire), selected(selected), parentSelected(parentSelected), flippedNormals(flippedNormals)
	{}

	float4x4 transform;
	const C_Mesh* mesh;
	const C_Material* material;
	bool shaded;
	bool wireframe;
	bool selected;
	bool parentSelected;
	bool flippedNormals;

};

template <typename Box>
struct RenderBox
{
	RenderBox(const Box* box, const Color& color) : box(box), color(color)
	{}

	const Box* box;
	Color color;
};

class M_Renderer3D : public Module
{
public:
	M_Renderer3D(bool start_enabled = true);
	~M_Renderer3D();

	bool Init(Config& config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void UpdateProjectionMatrix();

	void SetActiveCamera(C_Camera* camera);
	void SetCullingCamera(C_Camera* camera);
	void DrawAllScene();

	C_Camera* GetActiveCamera();

	void AddMesh(float4x4 transform, const C_Mesh* mesh, const C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals);
	void DrawAllMeshes();
	void DrawMesh(const RenderMesh& mesh);

	void AddAABB(const AABB& box, const Color& color);
	void AddOBB(const OBB& box, const Color& color);
	void AddFrustum(const Frustum& box, const Color& color);
	void DrawAllBox();

	//Component buffers management -----------------
	void LoadBuffers(R_Mesh* mesh);
	void LoadBuffers(C_Material* material);

	void ReleaseBuffers(R_Mesh* mesh);
	void ReleaseBuffers(C_Material* material);
	//----------------------------------------------

	void OnRemoveGameObject(GameObject* gameObject);
public:
	//TODO: should it be moved into window module? SDL method maybe?
	int window_width;
	int window_height;

	C_Camera* camera = nullptr;
	C_Camera* culling_camera = nullptr;

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
	uint mesh_draw_timer;
	uint box_draw_timer;

	std::vector<RenderMesh> meshes;
	
	std::vector<RenderBox<AABB>> aabb;
	std::vector<RenderBox<OBB>> obb;
	std::vector<RenderBox<Frustum>> frustum;
};