#ifndef  __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include "Color.h"

#include "ERenderingFlagsh.h"

#include "MathGeoLib\src\MathBuildConfig.h"
#include "MathGeoLib\src\MathGeoLib.h"

class GameObject;

class C_Camera : public Component
{
public:
	C_Camera(GameObject* gameObject);
	~C_Camera();

	//Frustum management ----
	inline float GetNearPlane() const { return frustum.NearPlaneDistance(); }
	inline float GetFarPlane() const { return frustum.FarPlaneDistance(); }
	inline float GetFOV() const { return hFov; }
	inline float GetSize() const { return size; }
	inline float GetAspectRatio() const { return frustum.AspectRatio(); }

	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFOV(float fov); 
	//void SetAspectRatio(float ar);
	void SetSize(float newSize);
	void SetResolution(float width, float height);

	void SetCameraAngle(EViewportCameraAngle::Flags);
	//--------------------------------

	LineSegment GenerateRaycast(float normalizedX, float normalizedY);
	void SetRenderingEnabled(bool enabled);

	void GenerateFrameBuffer();
	void UpdateFrameBufferSize();
	inline uint GetFrameBuffer() const { return frameBuffer; }
	inline uint GetRenderTarget() const { return renderTexture; }

	float4x4 GetOpenGLViewMatrix() const;
	float4x4 GetOpenGLProjectionMatrix() const;

	void OnTransformUpdated() override;

	virtual void Draw(ERenderingFlags::Flags flags) override;

	void Serialize(Config& config) override;

	static inline Type GetType() { return Type::Camera; };

public:
	bool		culling = false;
	bool		renderingEnabled = false;
	Plane		planes[6];
	float2		resolution;
	Color		backgroundColor;

	EViewportViewMode::Flags viewMode = EViewportViewMode::Lit;
	EViewportCameraAngle::Flags cameraAngle = EViewportCameraAngle::Perspective;
	ERenderingFlags::Flags renderingFlags = ERenderingFlags::DefaultGameFlags;

	float3 referencePoint = float3::zero;
private:
	Frustum		frustum;

	LineSegment lastRay;

	uint frameBuffer = 0;
	uint depthStencilBuffer = 0;
	uint renderTexture = 0;

	float hFov = 0.0f;
	float size = 5.0f;
};

#endif // __CAMERA_H__

