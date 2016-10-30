#ifndef __MODULE_SCENE_H__
#define __MODULE_SCENE_H__

#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
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
	
	void ResetScene();

	GameObject* getRoot();
	const GameObject* getRoot() const;

	void TestGameObjectsCulling(std::vector<GameObject*>& vector, GameObject* gameObject, bool lib = false, bool optimized = true);

public:
	bool reset = false;
	Timer timer;
	uint tmp_goCount = 1;

private:
	bool drawGrid = true;
	GameObject* root = nullptr;
	GameObject* camera = nullptr;

	uint cullingTimer_library = 0;
	uint cullingTimer_normal = 0;
	uint cullingTimer_optimized = 0;
};

#endif //__MODULE_SCENE_H__