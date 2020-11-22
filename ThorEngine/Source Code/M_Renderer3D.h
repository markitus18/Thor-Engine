#ifndef __M_RENDERER_H__
#define __M_RENDERER_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "ResourceHandle.h"
#include "ERenderingFlagsh.h"

//TODO: this should be removed or changed by float4x4
#include "MathGeoLib\src\MathGeoLib.h"
#include <map>

#define MAX_LIGHTS 8

class C_Camera;
class C_Mesh;
class C_Material;

class R_Mesh;
class R_Shader;
class R_Texture;
class R_Material;

class Config;

struct RenderMesh
{
	RenderMesh(const float4x4& trans, const C_Mesh* m, const C_Material* mat) : transform(trans), mesh(m), material(mat) {}

	float4x4 transform;
	const C_Mesh* mesh;
	const C_Material* material;
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
	RenderLine(const float3& a, const float3& b, const Color& color) : startPoint(a), endPoint(b), color(color) {}
	float3 startPoint;
	float3 endPoint;
	Color color;
};

struct RenderParticle
{
	RenderParticle(const float4x4& tr, R_Material* mat, float4 color) : transform(tr), mat(mat), color(color) {}

	float4x4 transform;
	R_Material* mat;
	float4 color;
};

struct CameraTarget
{
	CameraTarget(const C_Camera* camera, EViewportViewMode::Flags viewMode) : camera(camera), viewMode(viewMode){};

	const C_Camera* camera = nullptr;
	EViewportViewMode::Flags viewMode;

	std::vector<RenderMesh> meshes;
	std::map<float, RenderParticle> particles;

	std::vector<RenderBox<AABB>> aabb;
	std::vector<RenderBox<OBB>> obb;
	std::vector<RenderBox<Frustum>> frustum;
	std::vector<RenderLine> lines;
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

	void OnResize();

	void DrawTargetCamera(CameraTarget& camera);

	void BeginTargetCamera(const C_Camera* camera, EViewportViewMode::Flags viewMode);
	void EndTargetCamera();

	void AddMesh(const float4x4& transform, C_Mesh* mesh, const C_Material* material);
	void AddParticle(const float4x4& transform, R_Material* mat, float4 color, float distanceToCamera);
	void AddAABB(const AABB& box, const Color& color);
	void AddOBB(const OBB& box, const Color& color);
	void AddFrustum(const Frustum& box, const Color& color);
	void AddLine(const float3 a, const float3 b, const Color& color);

	void DrawAllMeshes(CameraTarget& cameraTarget);
	void DrawMesh(RenderMesh& mesh);

	void DrawAllParticles(CameraTarget& cameraTarget);
	void DrawParticle(RenderParticle& particle);

	void DrawAllBox(CameraTarget& cameraTarget);
	void DrawAllLines(CameraTarget& cameraTarget);

	//Component buffers management -----------------
	void LoadBuffers(R_Mesh* mesh);
	void LoadBuffers(C_Material* material);

	void ReleaseBuffers(R_Mesh* mesh);
	void ReleaseBuffers(R_Texture* texture);
	//----------------------------------------------

	uint SaveImage(const char* pathr);
	uint SaveModelThumbnail(GameObject* gameObject);

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	bool drawGrid = true;
	bool depthEnabled = true;

private:
	ResourceHandle<R_Texture> hDefaultTexture;
	ResourceHandle<R_Material> hDefaultMaterial;
	ResourceHandle<R_Shader> hDefaultShader;

	std::vector<CameraTarget> cameraRenderingTargets;
	CameraTarget* currentCameraTarget = nullptr;
};

#endif //__M_RENDERER_H__