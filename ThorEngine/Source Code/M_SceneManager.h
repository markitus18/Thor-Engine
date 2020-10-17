#ifndef __MODULE_SCENE_MANAGER_H__
#define __MODULE_SCENE_MANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include "ResourceHandle.h"

#include "MathGeoLib/src/Algorithm/Random/LCG.h"
#include "MathGeoLib/src/Geometry/LineSegment.h"

class GameObject;
class Config;
class Quadtree;
class C_Camera;
class R_Map;
class Scene;

class M_SceneManager : public Module
{
public:
	M_SceneManager(bool start_enabled = true);
	~M_SceneManager();

	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	GameObject* GetRoot();
	const GameObject* GetRoot() const;
	
	//Scene and prefab save / load ------------------------------------------------
	void SaveConfig(Config& config) const override;
	void LoadConfig(Config& config) override;

	//Generates a new scene and adds it to the list of active scenes
	//The scene can be loaded from a resource or start as an empty scene
	Scene* CreateNewScene(uint64 resourceID = 0);

	//Loads the current game scene with the specified map
	//'append' to merge the new map file with the current scene
	void LoadMap(const char* file, bool append = false);
	void LoadMap(uint64 resourceID, bool append = false);

	//Opens a model resource and adds it to the targetScene.
	//targetScene defaults to gameScene if not specified
	void LoadModel(uint64 resourceID, Scene* targetScene = nullptr);


	void StartSceneSimulation(Scene* scene);
	void StopSceneSimulation(Scene* scene);
	void PauseSceneSimulation(Scene* scene);

	//Removes a scene from the active scenes vector
	void RemoveScene(Scene* scene);

private:
	void AddRootToScene(GameObject* root, Scene* target);

public:
	bool drawQuadtree = false;
	bool drawBounds = false;
	bool drawBoundsSelected = false;
	  
	Scene* gameScene = nullptr;
	std::vector<Scene*> activeScenes;
};

#endif //__MODULE_SCENE_H__