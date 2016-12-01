#include "Application.h"

#include "Module.h"
#include "M_FileSystem.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Audio.h"
#include "M_Scene.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_Editor.h"
#include "M_Resources.h"
#include "M_Import.h"
#include "M_Materials.h"
#include "M_Meshes.h"
#include "Config.h"

#include "parson/parson.h"

Application::Application()
{
	fileSystem = new M_FileSystem();
	window = new M_Window();
	input = new M_Input();
	audio = new M_Audio();

	scene = new M_Scene();

	renderer3D = new M_Renderer3D();
	camera = new M_Camera3D();

	moduleResources = new M_Resources();
	moduleEditor = new M_Editor();
	moduleImport = new M_Import();
	moduleMaterials = new M_Materials();
	moduleMeshes = new M_Meshes();
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(camera);

	AddModule(moduleEditor);

	AddModule(input);
	AddModule(audio);

	AddModule(scene);


	AddModule(renderer3D);

	AddModule(moduleResources);
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

	char* buffer = nullptr;

	uint size = fileSystem->Load("ProjectSettings/Settings.JSON", &buffer);

	if (size <= 0)
	{
		uint defSize = fileSystem->Load("ProjectSettings/DefaultSettings.JSON", &buffer);
		if (defSize <= 0)
		{
			LOG("[error] failed to load project settings");
			return false;
		}
	}

	Config config(buffer);
	Config node = config.GetNode("EditorState");
	// Call Init() in all modules
	for (uint i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i]->IsEnabled())
			ret = list_modules[i]->Init(node.GetNode(list_modules[i]->name.c_str()));
	}

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

	//Time::Start(maxFPS);

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = frameTimer.ReadSec();
	frameTimer.Start();
	Time::PreUpdate(dt);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Time::Update();
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
	SaveSettingsNow("ProjectSettings/Settings.JSON");
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

void Application::UpdateSceneName()
{
	std::string windowTitle = title;
	std::string sceneName = "", sceneExtension = "";
	App->fileSystem->SplitFilePath(scene->current_scene.c_str(), nullptr, &sceneName, &sceneExtension);
	windowTitle.append(" - ").append(sceneName);
	if (sceneExtension != "")
	{
		windowTitle.append((".") + sceneExtension);
	}
	window->SetTitle(windowTitle.c_str());
}

void Application::SetTitleName(const char* new_name)
{
	title = new_name;
	UpdateSceneName();
}

void Application::OpenSceneWindow()
{

}

void Application::SaveScene(const char* scene, bool tmp)
{
	scene_to_save = scene;
	tmpScene = tmp;
	save_scene = true;
}

void Application::LoadScene(const char* scene)
{
	scene_to_load = scene;
	load_scene = true;
}

void Application::OnRemoveGameObject(GameObject* gameObject)
{
	for (uint i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->OnRemoveGameObject(gameObject);
	}
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

	for (uint i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->SaveScene(config.SetNode(list_modules[i]->name.c_str()));
	}

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);

	std::string extension = "";
	std::string full_path = "";

	fileSystem->SplitFilePath(scene_to_save.c_str(), nullptr, nullptr, &extension);

	if (extension != "scene")
		scene_to_save.append(".scene");
	
	if (tmpScene == false)
	{
		scene->current_scene = scene_to_save;
		UpdateSceneName();
	}

	fileSystem->Save(scene_to_save.c_str(), buffer, size);
	RELEASE_ARRAY(buffer);

	save_scene = false;
}

void Application::LoadSceneNow()
{
	char* buffer = nullptr;

	if (App->fileSystem->Exists(scene_to_load.c_str()))
	{
		uint size = App->fileSystem->Load(scene_to_load.c_str(), &buffer);

		if (size > 0)
		{
			if (tmpScene == false && scene_to_load.find("ProjectSettings/") == scene_to_load.npos)
				scene->current_scene = scene_to_load;
			
			Config config(buffer);

			for (uint i = 0; i < list_modules.size(); i++)
			{
				list_modules[i]->LoadScene(config.GetNode(list_modules[i]->name.c_str()));
			}
		}
		else
		{
			LOG("[error] File '%s' is empty", scene_to_load.c_str());
		}
	}
	else
	{
		LOG("[error] File '%s' not found", scene_to_load.c_str());
	}
	
	RELEASE_ARRAY(buffer);

	if (tmpScene == false)
		UpdateSceneName();

	load_scene = false;
	tmpScene = false;
}