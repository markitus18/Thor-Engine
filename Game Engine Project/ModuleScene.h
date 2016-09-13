#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Tower.h"
#include "Timer.h"

struct PhysBody3D;
struct PhysMotor3D;
class Tower;

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
	virtual void LoadPositions();
	void LoadTowers();
	void CheckTowersNumbers();

	void ResetScene();
	void LoadNextScene();
	void SelectScene();

	void SetNextScene(ModuleScene* scene);

public:
	int allyTowers = 0;
	int enemyTowers = 0;
	bool ended = false;
	bool reset = false;
	bool loadNext = false;

	int allyNum = 0;
	int enemyNum = 0;
	int neutralNum = 0;

	vec4* allyPositions = NULL;
	vec4* enemyPositions = NULL;
	vec4* neutralPositions = NULL;

	Timer timer;

	p2DynArray<Tower*> towers;

	ModuleScene* nextScene = NULL;
};