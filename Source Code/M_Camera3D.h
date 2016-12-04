#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include "Module.h"
#include "MathGeoLib\src\MathGeoLib.h"

class C_Camera;
class Config;

class M_Camera3D : public Module
{
public:
	M_Camera3D(bool start_enabled = true);
	~M_Camera3D();

	bool Init(Config& config);
	bool Start();
	update_status Update(float dt);
	bool CleanUp() override;

	float3 GetPosition() const;
	float3 GetReference() const;

	void Look(const float3& position);
	void CenterOn(const float3& position, float distance);
	void SetNewTarget(const float3& new_target);
	void Match(const C_Camera* camera);

	void SetPosition(float3 position);

	C_Camera* GetCamera() const;

	void SaveScene(Config& root) const;
	void LoadScene(Config& root, bool tmp = false);

private:

	void Move_Keyboard(float dt);
	void Move_Mouse();

	void Orbit(float motion_x, float motion_y);
	void Zoom(float zoom);

	void OnClick();

public:
	bool drawRay = false;

private:

	float3 reference;
	C_Camera* camera = nullptr;
	LineSegment lastRay;
};

#endif //__MODULE_CAMERA_H__