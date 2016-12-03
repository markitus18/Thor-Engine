#ifndef __MODULE_SCENE_H__
#define __MODULE_SCENE_H__

#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/src/Algorithm/Random/LCG.h"
#include "MathGeoLib/src/Geometry/LineSegment.h"

class GameObject;
class Config;
class Quadtree;

class M_Scene : public Module
{
public:
	M_Scene(bool start_enabled = true);
	~M_Scene();

	bool Init(Config& config);
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* GetRoot();
	const GameObject* GetRoot() const;

	void SetStaticGameObject(GameObject* gameObject, bool isStatic, bool allChilds);

	//Scene and prefab save / load ------------------------------------------------
	void SaveConfig(Config& config) const;
	void LoadConfig(Config& config);

	void SaveScene(Config& node) const;
	void LoadScene(Config& node); //Load itself
	void LoadScene(const char* file); //Calls application and prepares to load

	void LoadGameObject(uint64 ID); //Calls module import to load a Game Object file
	//Endof Scene and prefab save / load ------------------------------------------

	//GameObject management -------------------------------------------------------
	GameObject* CreateGameObject(const char* name);

	void DeleteGameObject(GameObject* gameObject);
	void OnRemoveGameObject(GameObject* gameObject);

	void OnClickSelection(const LineSegment& segment);
	//Endof GameObject management -------------------------------------------------

	//GameObject primitives creation ---------
	void CreateCamera();

	//----------------------------------------

	void Play();
	void Stop();

private:
	void TestGameObjectsCulling(std::vector<const GameObject*>& vector, std::vector<const GameObject*>& final);
	void UpdateAllGameObjects(GameObject* gameObject);
	void DrawAllGameObjects(GameObject* gameObject);
	void GettAllGameObjects(std::vector<GameObject*>& vector, GameObject* gameObject) const;
	void FindGameObjectByID(uint id, GameObject* gameObject, GameObject** ret);
	void DeleteAllGameObjects();
	void CreateDefaultScene();
	void DeleteToRemoveGameObjects();
public:
	bool reset = false;
	std::string current_scene = "Untitled";
	Quadtree* quadtree;
private:
	bool drawGrid = true;
	GameObject* root = nullptr;
	std::vector<const GameObject*> nonStatic;

	std::vector<GameObject*> toRemove;

	uint cullingTimer_library = 0;
	uint cullingTimer_normal = 0;
	uint cullingTimer_optimized = 0;

	LCG random;
};

#endif //__MODULE_SCENE_H__