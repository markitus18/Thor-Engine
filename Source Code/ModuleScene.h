#pragma once

#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Timer.h"

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include <list>

struct PhysBody3D;
struct PhysMotor3D;
class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	
	void ResetScene();

	void AddGameObject(char* mesh_path);

public:
	bool reset = false;
	Timer timer;

private:
	bool drawGrid = true;
	std::list<GameObject*> gameObjects;
};