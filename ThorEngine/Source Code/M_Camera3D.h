#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include "Module.h"
#include "MathGeoLib\src\MathGeoLib.h"

class C_Camera;
class Config;
struct Vec2;

class M_Camera3D : public Module
{
public:
	M_Camera3D(bool start_enabled = true);
	~M_Camera3D();

	bool Init(Config& config) override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	float3 GetPosition() const;
	float3 GetReference() const;

	void DrawRay();
private:


public:
	bool drawRay = false;

private:

	float3 reference;
	C_Camera* camera = nullptr;
	LineSegment lastRay;
};

#endif //__MODULE_CAMERA_H__