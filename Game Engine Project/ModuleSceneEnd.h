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

class ModuleSceneEnd : public ModuleScene
{
public:
	ModuleSceneEnd(Application* app, bool start_enabled = true);

	void LoadPositions();
};
