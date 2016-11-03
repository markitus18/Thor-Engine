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

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::SaveSettingsNow(const char* full_path)
{
	LOG("Saving Config State");

	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root = json_value_get_object(root_value);
	json_object_set_number(root, "Number", 15);

	
	for (uint i = 0; i < list_modules.size(); i++)
	{
		json_object_set_value(root, list_modules[i]->name.c_str(), json_value_init_object());
		list_modules[i]->Save(json_object_get_object(root, list_modules[i]->name.c_str()));
	}
	

	size_t size = json_serialization_size(root_value);
	char* buffer = new char[size];
	json_serialize_to_buffer(root_value, buffer, size);

	fileSystem->Save(full_path, buffer, size);

	RELEASE_ARRAY(buffer);
	json_value_free(root_value);
}

void Application::LoadSettingsNow(const char* full_path)
{
	char* buffer = nullptr;
	uint size = fileSystem->Load(full_path, &buffer);

	if (size > 0)
	{
		JSON_Value* root_value = json_parse_string(buffer);
		if (root_value)
		{
			JSON_Object* root = json_value_get_object(root_value);

			for (uint i = 0; i < list_modules.size(); i++)
			{
				list_modules[i]->Load(json_object_get_object(root, list_modules[i]->name.c_str()));
			}
		}
	}
}