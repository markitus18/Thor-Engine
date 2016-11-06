#include "Application.h"

#include "Module.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleImport.h"
#include "ModuleMaterials.h"
#include "ModuleMeshes.h"
#include "Config.h"

#include "parson/parson.h"

Application::Application()
{
	fileSystem = new ModuleFileSystem(this);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);

	scene = new ModuleScene(this, true);

	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);

	moduleEditor = new ModuleEditor(this);
	moduleImport = new ModuleImport(this);
	moduleMaterials = new ModuleMaterials(this);
	moduleMeshes = new ModuleMeshes(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);

	AddModule(scene);
	AddModule(moduleEditor);

	AddModule(renderer3D);

	AddModule(moduleImport);
	AddModule(moduleMaterials);
	AddModule(moduleMeshes);

	title = TITLE;
	organization = ORGANIZATION;
}

Application::~Application()
{
	for (uint i = 0; i < list_modules.size(); i++)
	{
		RELEASE(list_modules[i])
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (uint i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i]->IsEnabled())
			ret = list_modules[i]->Init();
	}

	LoadSettingsNow("Config.JSON");

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	for (uint i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i]->IsEnabled())
			ret = list_modules[i]->Start();
	}
	
	//Setting up all timers
	frameTimer.Start();
	frame_ms_cap = (float)1000 / (float)maxFPS;

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = frameTimer.ReadSec();
	frameTimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	float frame_ms = frameTimer.Read();
	if (frame_ms > 0 && frame_ms < frame_ms_cap)
	{
		SDL_Delay(frame_ms_cap - frame_ms);
	}

	frame_count++;
	if (second_count.Read() >= 1000)
	{
		second_count.Start();
		last_FPS = frame_count;
		frame_count = 0;
	}

	App->moduleEditor->UpdateFPSData(last_FPS, frameTimer.Read());

	if (save_scene)
		SaveSceneNow();
	if (load_scene)
		LoadSceneNow();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}

	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}

	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	SaveSettingsNow("Config.JSON");
	for (uint i = 0; i < list_modules.size(); i++)
	{
		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::RequestBrowser(char* path)
{
	ShellExecuteA(0, "Open", path, 0, "", 3);
}


void Application::Log(const char* input)
{
	moduleEditor->Log(input);
}

const char* Application::GetTitleName() const
{
	return title.c_str();
}

const char* Application::GetOrganizationName() const
{
	return organization.c_str();
}

void Application::SetTitleName(const char* new_name)
{
	title = new_name;
	window->SetTitle(new_name);
}

void Application::SaveScene(const char* scene)
{
	scene_to_save = scene;
	save_scene = true;
}

void Application::LoadScene(const char* scene)
{
	scene_to_load = scene;
	load_scene = true;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::SaveSettingsNow(const char* full_path)
{
	LOG("Saving Config State");

	Config config;
	Config node = config.SetNode("EditorState");

	for (uint i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->SaveConfig(node.SetNode(list_modules[i]->name.c_str()));
	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);

	fileSystem->Save(full_path, buffer, size);
	RELEASE_ARRAY(buffer);
}

void Application::LoadSettingsNow(const char* full_path)
{
	char* buffer = nullptr;
	uint size = fileSystem->Load(full_path, &buffer);

	if (size > 0)
	{
		Config config(buffer);
		Config root = config.GetNode("EditorState");
		if (config.NodeExists())
		{
			for (uint i = 0; i < list_modules.size(); i++)
			{
				list_modules[i]->LoadConfig(root.GetNode(list_modules[i]->name.c_str()));
			}
		}
	}
}

void Application::SaveSceneNow()
{
	Config config;
	Config node = config.SetNode(scene_to_save.c_str());

	for (uint i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->SaveScene(node.SetNode(list_modules[i]->name.c_str()));
	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);

	std::string full_path = scene_to_save;
	full_path.append(".scene");

	fileSystem->Save(full_path.c_str(), buffer, size);
	RELEASE_ARRAY(buffer);

	save_scene = false;
}

void Application::LoadSceneNow()
{
	char* buffer = nullptr;
	std::string fullPath = scene_to_load;
	fullPath.append(".scene");

	if (App->fileSystem->Exists(fullPath.c_str()))
	{
		uint size = App->fileSystem->Load(fullPath.c_str(), &buffer);

		if (size > 0)
		{
			Config config(buffer);
			Config node = config.GetNode(scene_to_load.c_str());

			for (uint i = 0; i < list_modules.size(); i++)
			{
				list_modules[i]->LoadScene(node.GetNode(list_modules[i]->name.c_str()));
			}
		}
		else
		{
			LOG("[error] File '%s' is empty", fullPath.c_str());
		}
	}
	else
	{
		LOG("[error] File '%s' not found", fullPath.c_str());
	}

	RELEASE_ARRAY(buffer);
	load_scene = false;
}