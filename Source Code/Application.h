#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "Time.h"
#include "PerfTimer.h"
#include <string>
#include <vector>

class Module;

class M_FileSystem;
class M_Window;
class M_Input;
class M_Audio;
class M_Scene;

class M_Renderer3D;
class M_Camera3D;
class M_Editor;
class M_Import;
class M_Materials;
class M_Meshes;
class M_Resources;

class GameObject;

class Application
{
public:
	M_FileSystem* fileSystem;
	M_Window* window = nullptr;
	M_Input* input = nullptr;
	M_Audio* audio = nullptr;
	M_Scene* scene = nullptr;

	M_Renderer3D* renderer3D = nullptr;
	M_Camera3D* camera = nullptr;

	M_Editor* moduleEditor = nullptr;

	M_Resources* moduleResources = nullptr;
	M_Import* moduleImport = nullptr;
	M_Materials* moduleMaterials = nullptr;
	M_Meshes* moduleMeshes = nullptr;

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