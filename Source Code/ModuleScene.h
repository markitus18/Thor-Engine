#ifndef __MODULE_SCENE_H__
#define __MODULE_SCENE_H__

#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Timer.h"

//#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/Algorithm/Random/LCG.h"

#include <list>

class GameObject;
class Config;
class Quadtree;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init(Config& config);
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* GetRoot();
	const GameObject* GetRoot() const;

	void SetStaticGameObject(GameObject* gameObject, bool isStatic, bool allChilds);

	void SaveConfig(Config& config) const;
	void LoadConfig(Config& config);

	void SaveScene(Config& node) const;
	void LoadScene(Config& node); //Load itself
	void LoadScene(const char* file); //Calls application and prepares to load

	void LoadGameObject(const char* file); //Calls module import to load a Game Object file

	GameObject* CreateGameObject(const char* name);
	void DeleteToRemoveGameObjects();
	bool DeleteGameObject(GameObject* gameObject);

	//GameObject primitives creation ---------
	void CreateCamera();

	//----------------------------------------
private:
	void TestGameObjectsCulling(std::vector<const GameObject*>& vector, std::vector<const GameObject*>& final);
	void DrawAllGameObjects(GameObject* gameObject);
	void GettAllGameObjects(std::vector<GameObject*>& vector, GameObject* gameObject) const;
	void FindGameObjectByID(uint id, GameObject* gameObject, GameObject** ret);
	void DeleteAllGameObjects();
	void CreateDefaultScene();

public:
	bool reset = false;
	std::string current_scene = "Untitled";
	Quadtree* quadtree;
private:
	bool drawGrid = true;
	GameObject* root = nullptr;



	uint cullingTimer_library = 0;
	uint cullingTimer_normal = 0;
	uint cullingTimer_optimized = 0;

	LCG random;
};

#endif //__MODULE_SCENE_H__