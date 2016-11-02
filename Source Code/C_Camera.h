#ifndef  __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"

#include "MathGeoLib\src\MathBuildConfig.h"
#include "MathGeoLib\src\MathGeoLib.h"
class GameObject;

class C_Camera : public Component
{
public:
	C_Camera(GameObject* gameObject);
	~C_Camera();

	//Frustum variable management ----
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;

	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFOV(float fov); 
	void SetAspectRatio(float ar);
	//--------------------------------


	void Look(const float3& position);
	void Match(const C_Camera* reference);

	float* GetOpenGLViewMatrix();
	float* GetOpenGLProjectionMatrix();

	void OnUpdateTransform(const float4x4& global, const float4x4& parent_global = float4x4::identity);

	void Save();
	void Load();

	static Component::Type GetType();

private:
	void UpdatePlanes();

public:
	Frustum		frustum;
	bool		update_projection = true;
	bool		culling = false;
	bool		active_camera = false;
	Plane		planes[6];
};

#endif // __CAMERA_H__

