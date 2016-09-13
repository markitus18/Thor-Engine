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

class ModuleScene3 : public ModuleScene
{
public:
	ModuleScene3(Application* app, bool start_enabled = true);

	void LoadPositions();
};
