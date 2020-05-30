#ifndef __M_RENDERER_H__
#define __M_RENDERER_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"

//TODO: this should be removed or changed by float4x4
#include "MathGeoLib\src\MathGeoLib.h"
#include <map>

#define MAX_LIGHTS 8

class C_Camera;
class C_Mesh;
class C_Material;

class R_Mesh;
class R_Texture;

class Config;

struct RenderMesh
{
	RenderMesh(const float4x4& trans, const C_Mesh* m, const C_Material* mat, bool sh, bool wire, bool selected, bool parentSelected, bool flippedNormals) : transform(trans), mesh(m), material(mat),
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

struct RenderParticle
{
	RenderParticle(const float4x4& tr, uint64 mat, float4 color) : transform(tr), materialID(mat), color(color) {}

	float4x4 transform;
	uint64 materialID;
	float4 color;
};

class M_Renderer3D : public Module
{
public:
	M_Renderer3D(bool start_enabled = true);
	~M_Renderer3D();

	bool Init(Config& config) override;
	bool Start() override;
	update_status PreUpdate() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void GenerateSceneBuffers();

	void OnResize();
	void UpdateProjectionMatrix();

	void SetActiveCamera(C_Camera* camera);
	void SetCullingCamera(C_Camera* camera);
	void DrawAllScene();

	C_Camera* GetActiveCamera();

	void AddMesh(const float4x4& transform, C_Mesh* mesh, const C_Material* material, bool shaded, bool wireframe, bool selected, bool parentSelected, bool flippedNormals);
	void DrawAllMeshes();
	void DrawMesh(RenderMesh& mesh);

	void AddParticle(const float4x4& transform, uint64 material, float4 color, float distanceToCamera);
	void DrawAllParticles();
	void DrawParticle(RenderParticle& particle);

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

	void OnRemoveGameObject(GameObject* gameObject) override;

	uint SaveImage(const char* pathr);
	uint SaveModelThumbnail(GameObject* gameObject);

	//Rendering Settings ---------------------------
	void SetDepthBufferEnabled(bool enabled);


	//----------------------------------------------

public:
	C_Camera* camera = nullptr;
	C_Camera* culling_camera = nullptr;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	bool drawGrid = true;

	uint frameBuffer = 0;
	uint depthBuffer = 0;
	uint renderTexture = 0;

	bool depthEnabled = true;

private:
	uint defaultTextureID = 0;

	uint mesh_draw_timer;
	uint particles_draw_timer;
	uint box_draw_timer;

	std::vector<RenderMesh> meshes;
	std::map<float, RenderParticle> particles;

	std::vector<RenderBox<AABB>> aabb;
	std::vector<RenderBox<OBB>> obb;
	std::vector<RenderBox<Frustum>> frustum;
	std::vector<RenderLine> lines;
};

#endif //__M_RENDERER_H__