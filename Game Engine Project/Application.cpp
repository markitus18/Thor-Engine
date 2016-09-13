#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);

	player = new ModulePlayer(this);

	scene_1 = new ModuleScene1(this, true);
	scene_2 = new ModuleScene2(this, false);
	scene_3 = new ModuleScene3(this, false);
	scene_4 = new ModuleScene4(this, false);
	scene_5 = new ModuleScene5(this, false);
	scene_6 = new ModuleScene6(this, false);
	scene_7 = new ModuleScene7(this, false);
	scene_end = new ModuleSceneEnd(this, false);

	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_1);
	AddModule(scene_2);
	AddModule(scene_3);
	AddModule(scene_4);
	AddModule(scene_5);
	AddModule(scene_6);
	AddModule(scene_7);
	AddModule(scene_end);

	AddModule(player);

	// Renderer last!
	AddModule(renderer3D);

	scene_1->SetNextScene((ModuleScene*)scene_2);
	scene_2->SetNextScene((ModuleScene*)scene_3);
	scene_3->SetNextScene((ModuleScene*)scene_4);
	scene_4->SetNextScene((ModuleScene*)scene_5);
	scene_5->SetNextScene((ModuleScene*)scene_6);
	scene_6->SetNextScene((ModuleScene*)scene_7);
	scene_7->SetNextScene((ModuleScene*)scene_end);
	scene_end->SetNextScene((ModuleScene*)scene_1);
}

Application::~Application()
{
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}
}

bool Application::Init()
{
	bool ret = true;
	totalTimer.Start();
	// Call Init() in all modules
	p2List_item<Module*>* item = list_modules.getFirst();

	while(item != NULL && ret == true)
	{
		if (item->data->IsEnabled())
			ret = item->data->Init();
		item = item->next;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.getFirst();

	while(item != NULL && ret == true)
	{
		if (item->data->IsEnabled())
			ret = item->data->Start();
		item = item->next;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	p2List_item<Module*>* item = list_modules.getFirst();
	
	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		if (item->data->IsEnabled())
		{
			ret = item->data->PreUpdate(dt);
		}
		item = item->next;
	}

	item = list_modules.getFirst();

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		if (item->data->IsEnabled())
		{
			ret = item->data->Update(dt);
		}
		item = item->next;
	}

	item = list_modules.getFirst();

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		if (item->data->IsEnabled())
		{
			ret = item->data->PostUpdate(dt);
		}
		item = item->next;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.add(mod);
}