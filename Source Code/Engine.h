#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Timer.h"
#include "PerfTimer.h"
#include <string>
#include <vector>

class Module;

class M_FileSystem;
class M_Window;
class M_Input;
class M_Scene;

class M_Renderer3D;
class M_Camera3D;
class M_Editor;
class M_Resources;
class M_Shaders;

class GameObject;

class TEngine
{
public:
	M_FileSystem* fileSystem = nullptr;
	M_Window* window = nullptr;
	M_Input* input = nullptr;
	M_Scene* scene = nullptr;

	M_Renderer3D* renderer3D = nullptr;
	M_Camera3D* camera = nullptr;

	M_Editor* moduleEditor = nullptr;

	M_Resources* moduleResources = nullptr;
	M_Shaders* moduleShaders = nullptr;

private:

	int			maxFPS = 0;
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

public:

	TEngine();
	~TEngine();


	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(char* path);
	void Log(const char* input);

	const char* GetTitleName() const;
	const char* GetOrganizationName() const;

	void UpdateSceneName();
	void SetTitleName(const char* new_name);

	void OnRemoveGameObject(GameObject* gameObject);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	void SaveSettingsNow(const char*);
	void LoadSettingsNow(const char*);
};

extern TEngine* Engine;

#endif //__APPLICATION_H__