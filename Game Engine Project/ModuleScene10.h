#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Tower.h"

struct PhysBody3D;
struct PhysMotor3D;
class Tower;

class ModuleScene10 : public ModuleScene
{
public:
	ModuleScene10(Application* app, bool start_enabled = true);

	void LoadPositions();
};
