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

	uint64 LoadScene(const char* file, bool append = false);
	void LoadScene(uint64 resourceID, bool append = false);
	void LoadModel(uint64 resourceID); //Calls module import to load a Game Object file
	//Endof Scene and model save / load ------------------------------------------

	void StartSceneSimulation(Scene* scene);
	void StopSceneSimulation(Scene* scene);
	void PauseSceneSimulation(Scene* scene);

	//Removes a scene from the active scenes vector
	void RemoveScene(Scene* scene);

public:
	bool drawQuadtree = false;
	bool drawBounds = false;
	bool drawBoundsSelected = false;

	Scene* gameScene = nullptr;
	std::vector<Scene*> activeScenes;
};

#endif //__MODULE_SCENE_H__