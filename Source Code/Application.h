#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include <string>
#include <vector>

class Module;

class M_FileSystem;
class ModuleWindow;
class ModuleInput;
class M_Audio;
class ModuleScene;

class ModuleRenderer3D;
class M_Camera3D;
class M_Editor;
class ModuleImport;
class ModuleMaterials;
class ModuleMeshes;

class GameObject;

class Application
{
public:
	M_FileSystem* fileSystem;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	M_Audio* audio = nullptr;
	ModuleScene* scene = nullptr;

	ModuleRenderer3D* renderer3D = nullptr;
	M_Camera3D* camera = nullptr;

	M_Editor* moduleEditor = nullptr;
	ModuleImport* moduleImport = nullptr;
	ModuleMaterials* moduleMaterials = nullptr;
	ModuleMeshes* moduleMeshes = nullptr;

private:

	int			maxFPS = 60;
	float		frame_ms_cap = 0.0f;

	Timer		frameTimer;
	float		dt;

	//FPS
	Timer		second_count;
	int			frame_count;
	int			last_FPS;

	std::vector<Module*> list_modules;
	
	std::string title;
	std::string organization;

	bool save_scene = false;
	bool load_scene = false;

	std::string scene_to_save;
	std::string scene_to_load;

public:

	Application();
	~Application();


	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(char* path);
	void Log(const char* input);

	const char* GetTitleName() const;
	const char* GetOrganizationName() const;

	void UpdateSceneName();
	void SetTitleName(const char* new_name);

	void OpenSceneWindow();
	void SaveScene(const char* file);
	void LoadScene(const char* file);

	void OnRemoveGameObject(GameObject* gameObject);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	void SaveSettingsNow(const char*);
	void LoadSettingsNow(const char*);

	void SaveSceneNow();
	void LoadSceneNow();
};

extern Application* App;

#endif //__APPLICATION_H__