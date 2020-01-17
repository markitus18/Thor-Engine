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
class C_Camera;

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
	
	std::string GetNewGameObjectName(const char* name, GameObject* parent = nullptr) const;
	int GetGameObjectNameCount(const char* name, GameObject* parent = nullptr) const;

	void SetStaticGameObject(GameObject* gameObject, bool isStatic, bool allChilds);

	//Scene and prefab save / load ------------------------------------------------
	void SaveConfig(Config& config) const;
	void LoadConfig(Config& config);

	void SaveScene(Config& node) const;
	void LoadScene(Config& node, bool tmp = false); //Load itself
	void LoadScene(const char* file); //Calls application and prepares to load

	void LoadGameObject(const Config& file); //Calls module import to load a Game Object file
	//Endof Scene and prefab save / load ------------------------------------------

	//GameObject management -------------------------------------------------------
	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	void DeleteGameObject(GameObject* gameObject);
	void OnRemoveGameObject(GameObject* gameObject);

	void OnClickSelection(const LineSegment& segment);
	//Endof GameObject management -------------------------------------------------

	//GameObject primitives creation ---------
	GameObject* CreateCamera();

	//----------------------------------------

	const C_Camera* GetMainCamera() const { return mainCamera; };

	void Play();
	void Stop();

	void CreateDefaultScene();

private:
	void TestGameObjectsCulling(std::vector<const GameObject*>& vector, std::vector<const GameObject*>& final);
	void UpdateAllGameObjects(GameObject* gameObject, float dt);
	void DrawAllGameObjects(GameObject* gameObject);
	void FindGameObjectByID(uint id, GameObject* gameObject, GameObject** ret);
	void DeleteAllGameObjects();

	void DeleteToRemoveGameObjects();

public:
	bool drawQuadtree = false;
	bool drawBounds = false;
	bool drawBoundsSelected = false;

	bool reset = false;
	std::string current_scene = "Untitled";
	Quadtree* quadtree;

private:
	GameObject* root = nullptr;
	std::vector<const GameObject*> nonStatic;
	std::vector<GameObject*> toRemove;

	uint cullingTimer_library = 0;
	uint cullingTimer_normal = 0;
	uint cullingTimer_optimized = 0;

	LCG random;

	C_Camera* mainCamera = nullptr;
};

#endif //__MODULE_SCENE_H__