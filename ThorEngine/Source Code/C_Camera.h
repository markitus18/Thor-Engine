#ifndef  __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include "Color.h"

#include "MathGeoLib\src\MathBuildConfig.h"
#include "MathGeoLib\src\MathGeoLib.h"

class GameObject;

class C_Camera : public Component
{
public:
	C_Camera(GameObject* gameObject);
	~C_Camera();

	//Frustum management ----
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;

	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFOV(float fov); 
	void SetAspectRatio(float ar);
	void SetResolution(float width, float height);
	//--------------------------------

	void Look(const float3& position);
	void Match(const C_Camera* reference);

	void SetRenderingEnabled(bool enabled);

	void GenerateFrameBuffer();
	void UpdateFrameBufferSize();
	inline uint GetFrameBuffer() const { return frameBuffer; }
	inline uint GetRenderTarget() const { return renderTexture; }

	float* GetOpenGLViewMatrix() const;
	float* GetOpenGLProjectionMatrix() const;

	void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	virtual void Draw(RenderingSettings::RenderingFlags flags) override;

	void Save();
	void Load();

	static inline Type GetType() { return Type::Camera; };

private:
	void UpdatePlanes();

public:
	Frustum		frustum;
	bool		culling = false;
	bool		renderingEnabled = false;
	Plane		planes[6];
	float2		resolution;
	Color		backgroundColor;

private:
	uint frameBuffer = 0;
	uint depthStencilBuffer = 0;
	uint renderTexture = 0;
};

#endif // __CAMERA_H__

