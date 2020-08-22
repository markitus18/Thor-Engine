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
class R_Scene;

class M_Scene : public Module
{
public:
	M_Scene(bool start_enabled = true);
	~M_Scene();

	bool Init(Config& config) override;
	bool Start() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	GameObject* GetRoot();
	const GameObject* GetRoot() const;
	
	std::string GetNewGameObjectName(const char* name, GameObject* parent = nullptr) const;
	int GetGameObjectNameCount(const char* name, GameObject* parent = nullptr) const;

	void SetStaticGameObject(GameObject* gameObject, bool isStatic, bool allChilds);

	//Scene and prefab save / load ------------------------------------------------
	void SaveConfig(Config& config) const override;
	void LoadConfig(Config& config) override;

	uint64 LoadScene(const char* file); //Calls application and prepares to load

	void LoadModel(uint64 resourceID); //Calls module import to load a Game Object file
	//Endof Scene and model save / load ------------------------------------------

	//GameObject management -------------------------------------------------------
	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	void DeleteGameObject(GameObject* gameObject);
	void OnRemoveGameObject(GameObject* gameObject);

	void OnClickSelection(const LineSegment& segment);
	//Endof GameObject management -------------------------------------------------

	//GameObject primitives creation ---------
	GameObject* CreateCamera();

	//----------------------------------------

	const C_Camera* GetMainCamera() const;

	void Play();
	void Stop();

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
	Quadtree* quadtree;

	R_Scene* scene = nullptr;
private:
	std::vector<const GameObject*> nonStatic;
	std::vector<GameObject*> toRemove;

	uint64 tmpSceneID = 0;

	LCG random;
};

#endif //__MODULE_SCENE_H__