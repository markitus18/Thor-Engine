#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"

class Module;

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleScene;

class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModuleImport;
class ModuleMaterials;

class Application
{
public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleScene* scene = nullptr;

	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;

	ModuleEditor* moduleEditor = nullptr;
	ModuleImport* moduleImport = nullptr;
	ModuleMaterials* moduleMaterials = nullptr;

private:

	int			maxFPS = 60;
	float		frame_ms_cap = 0.0f;

	Timer		frameTimer;
	float		dt;

	//FPS
	Timer		second_count;
	int			frame_count;
	int			last_FPS;

	p2List<Module*> list_modules;

public:

	Application();
	~Application();


	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(char* path);
	void Log(const char* input);

	const char* GetWindowTitle() const;
	void SetWindowTitle(char* new_name);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif //__APPLICATION_H__