#pragma once

#include "Primitive.h"
#include "PhysBody3D.h"
#include "Application.h"
#include "ModulePhysics3D.h"

enum TOWER_TYPE
{
	TOWER_ALLY = 0,
	TOWER_ENEMY,
	TOWER_NEUTRAL,
};

class Tower
{
public:
	Tower();
	Tower(float x, float y, float z, TOWER_TYPE type, float height = 10, float cubeSize = 2);
	bool Update();

	bool isDestroyed();
	void ChangeColor(Color);
	Color GetColor(TOWER_TYPE);


public:
	p2DynArray<Cube*> cubes;
	p2DynArray<PhysBody3D*> pbs;
	p2DynArray<vec3> positions;

	TOWER_TYPE type;
	bool fallen = false;
};
