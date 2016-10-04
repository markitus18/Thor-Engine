#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleImport.h"
#include "GameObject.h"
#include <gl/GL.h>
#include <gl/GLU.h>
//#include <stdio.h>

//#include <GLFW/glfw3.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	AddGameObject("Game/RandomStuff.fbx");

	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	timer.Start();
	timer.Stop();
	reset = false;
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	if (drawGrid)
	{
		P_Plane p(0, 0, 0, 1);
		p.axis = true;
		p.Render();
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		drawGrid = !drawGrid;
	}

	std::list<GameObject*>::iterator it = gameObjects.begin();
	
	while (it != gameObjects.end())
	{
		(*it)->mesh.Draw();
		it++;;
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	if (reset)
	{
		ResetScene();
		reset = false;
	}
	return UPDATE_CONTINUE;
}

void ModuleScene::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

void ModuleScene::ResetScene()
{
	App->camera->Disable();
	this->Disable();
	this->Enable();
	App->camera->Enable();
}

void ModuleScene::AddGameObject(char* mesh_path)
{
	GameObject* gameObject = App->moduleImport->LoadFBX(mesh_path);
	if (gameObject)
	{
		gameObjects.push_back(gameObject);
	}
}