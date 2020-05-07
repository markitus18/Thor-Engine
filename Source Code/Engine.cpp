#include "Engine.h"

#include "Module.h"
#include "M_FileSystem.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Scene.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_Editor.h"
#include "M_Resources.h"
#include "I_Shaders.h"

#include "Config.h"

#include "parson/parson.h"
#include "Brofiler/Brofiler.h"

TEngine::TEngine()
{
	fileSystem = new M_FileSystem();
	window = new M_Window();
	input = new M_Input();

	scene = new M_Scene();

	renderer3D = new M_Renderer3D();
	camera = new M_Camera3D();

	moduleResources = new M_Resources();
	moduleEditor = new M_Editor();

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(input);

	AddModule(camera);
	AddModule(scene);

	AddModule(moduleResources);

	AddModule(renderer3D);
	AddModule(moduleEditor);

	title = TITLE;
	organization = ORGANIZATION;
}

TEngine::~TEngine()
{
	for (uint i = 0; i < list_modules.size(); i++)
	{
		RELEASE(list_modules[i])
	}
}

bool TEngine::Init()
{
	bool ret = true;

	char* buffer = nullptr;

	uint size = fileSystem->Load("Engine/Settings.JSON", &buffer);

	if (size <= 0)
	{
		uint defSize = fileSystem->Load("Engine/DefaultSettings.JSON", &buffer);
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
		if (list_modules[i]->IsActive())
			ret = list_modules[i]->Init(node.GetNode(list_modules[i]->name.c_str()));
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	for (uint i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i]->IsActive())
			ret = list_modules[i]->Start();
	}
	
	//Setting up all timers
	frameTimer.Start();

	if (maxFPS > 0)
		frame_ms_cap = (float)1000 / (float)maxFPS;

	//Time::Start(maxFPS);

	return ret;
}

// ---------------------------------------------
void TEngine::PrepareUpdate()
{
	dt = frameTimer.ReadSec();
	frameTimer.Start();
	Time::PreUpdate(dt);
}

// ---------------------------------------------
void TEngine::FinishUpdate()
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

	Engine->moduleEditor->UpdateFPSData(last_FPS, frameTimer.Read());

	if (save_scene)
		SaveSceneNow();
	if (load_scene)
		LoadSceneNow();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status TEngine::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	BROFILER_CATEGORY("App PreUpdate", Profiler::Color::Yellow)
	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}
	BROFILER_CATEGORY("App Update", Profiler::Color::Purple)
	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}
	BROFILER_CATEGORY("App PostUpdate", Profiler::Color::Green)
	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool TEngine::CleanUp()
{
	bool ret = true;
	SaveSettingsNow("Engine/Settings.JSON");
	for (uint i = 0; i < list_modules.size(); i++)
	{
		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void TEngine::RequestBrowser(char* path)
{
	ShellExecuteA(0, "Open", path, 0, "", 3);
}


void TEngine::Log(const char* input)
{
	moduleEditor->Log(input);
}

const char* TEngine::GetTitleName() const
{
	return title.c_str();
}

const char* TEngine::GetOrganizationName() const
{
	return organization.c_str();
}

void TEngine::UpdateSceneName()
{
	std::string windowTitle = title;
	std::string sceneName = "", sceneExtension = "";
	Engine->fileSystem->SplitFilePath(scene->current_scene.c_str(), nullptr, &sceneName, &sceneExtension);
	windowTitle.append(" - ").append(sceneName);
	if (sceneExtension != "")
	{
		windowTitle.append((".") + sceneExtension);
	}
	window->SetTitle(windowTitle.c_str());
}

void TEngine::SetTitleName(const char* new_name)
{
	title = new_name;
	UpdateSceneName();
}

void TEngine::OpenSceneWindow()
{

}

void TEngine::SaveScene(const char* scene, bool tmp)
{
	scene_to_save = tmp ? "" : "Assets/";
	scene_to_save += scene;
	tmpScene = tmp;
	save_scene = true;
}

void TEngine::LoadScene(const char* scene)
{
	scene_to_load = scene;
	load_scene = true;
}

void TEngine::OnRemoveGameObject(GameObject* gameObject)
{
	for (uint i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->OnRemoveGameObject(gameObject);
	}
}

void TEngine::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void TEngine::SaveSettingsNow(const char* full_path)
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

void TEngine::LoadSettingsNow(const char* full_path)
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

void TEngine::SaveSceneNow()
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

void TEngine::LoadSceneNow()
{
	char* buffer = nullptr;

	if (Engine->fileSystem->Exists(scene_to_load.c_str()))
	{
		uint size = Engine->fileSystem->Load(scene_to_load.c_str(), &buffer);

		if (size > 0)
		{
			if (tmpScene == false && scene_to_load.find("ProjectSettings/") == scene_to_load.npos)
				scene->current_scene = scene_to_load;
			
			Config config(buffer);

			for (uint i = 0; i < list_modules.size(); i++)
			{
				list_modules[i]->LoadScene(config.GetNode(list_modules[i]->name.c_str()), tmpScene);
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