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
	ModuleWindow* window = NULL;
	ModuleInput* input = NULL;
	ModuleAudio* audio = NULL;
	ModuleScene* scene = NULL;

	ModuleRenderer3D* renderer3D = NULL;
	ModuleCamera3D* camera = NULL;

	ModuleEditor* moduleEditor = NULL;
	ModuleImport* moduleImport = NULL;
	ModuleMaterials* moduleMaterials = NULL;

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