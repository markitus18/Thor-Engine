#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib\src\MathGeoLib.h"

class C_Camera;

class ModuleCamera : public Module
{
public:
	ModuleCamera(Application* app, bool start_enabled = true);
	~ModuleCamera();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp() override;

	float3 GetPosition() const;
	void Look(const float3& position);
	void CenterOn(const float3& position, float distance);

	C_Camera* GetCamera() const;

private:

	void Move_Keyboard(float dt);
	void Move_Mouse();

	void Orbit(float motion_x, float motion_y);
	void Zoom(float zoom);

private:

	float3 reference;
	C_Camera* camera = NULL;
};

#endif //__MODULE_CAMERA_H__