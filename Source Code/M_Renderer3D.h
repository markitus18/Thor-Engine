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
class R_Texture;

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

struct RenderLine
{
	RenderLine(const float3& a, const float3& b, const Color& color) : start(a), end(b), color(color) {}
	float3 start;
	float3 end;
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
	void GenerateSceneBuffers();

	void OnResize();
	void UpdateProjectionMatrix();

	void SetActiveCamera(C_Camera* camera);
	void SetCullingCamera(C_Camera* camera);
	void DrawAllScene();

	C_Camera* GetActiveCamera();

	void AddMesh(float4x4 transform, C_Mesh* mesh, const C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals);
	void DrawAllMeshes();
	void DrawMesh(RenderMesh& mesh);

	void AddAABB(const AABB& box, const Color& color);
	void AddOBB(const OBB& box, const Color& color);
	void AddFrustum(const Frustum& box, const Color& color);
	void DrawAllBox();

	void AddLine(const float3 a, const float3 b, const Color& color);
	void DrawAllLines();

	//Component buffers management -----------------
	void LoadBuffers(R_Mesh* mesh);
	void LoadBuffers(C_Material* material);

	void ReleaseBuffers(R_Mesh* mesh);
	void ReleaseBuffers(R_Texture* texture);
	//----------------------------------------------

	void OnRemoveGameObject(GameObject* gameObject);

	uint SaveImage(const char* pathr);
	uint SavePrefabImage(GameObject* gameObject);

	//Rendering Settings ---------------------------
	void SetDepthBufferEnabled(bool enabled);


	//----------------------------------------------

public:
	C_Camera* camera = nullptr;
	C_Camera* screenshotCamera = nullptr;
	C_Camera* culling_camera = nullptr;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	bool drawGrid = true;

	uint frameBuffer = 0;
	uint depthBuffer = 0;
	uint renderTexture = 0;

	bool depthEnabled = false;

private:
	uint mesh_draw_timer;
	uint box_draw_timer;

	std::vector<RenderMesh> meshes;
	
	std::vector<RenderBox<AABB>> aabb;
	std::vector<RenderBox<OBB>> obb;
	std::vector<RenderBox<Frustum>> frustum;
	std::vector<RenderLine> lines;
};