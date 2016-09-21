#pragma once

#include "Glew/include/glew.h"

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"

class Module;

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleScene;

class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class ModuleUI;

class Application
{
public:
	ModuleWindow* window = NULL;
	ModuleInput* input = NULL;
	ModuleAudio* audio = NULL;
	ModuleScene* scene = NULL;

	ModuleRenderer3D* renderer3D = NULL;
	ModuleCamera3D* camera = NULL;
	ModulePhysics3D* physics = NULL;

	ModuleUI* moduleUI = NULL;

private:

	Timer	ms_timer;
	float	dt;
	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	Timer totalTimer;
	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(char* path);
	void Log(const char* input);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;