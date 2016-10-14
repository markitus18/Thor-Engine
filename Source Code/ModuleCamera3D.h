#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void SetNewTarget(const vec3 &new_target);

	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:
	
	float maxDistanceToVehicle = 25.0f;
	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};

#endif //__MODULE_CAMERA_H__