#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"

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

	bool ret = true;

	grid.axis = true;

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
	grid.Render();

	bool toClear = false;
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			toClear = true;
			delete (gameObjects[i]);
			gameObjects[i] = NULL;
		}
		else
		{
			if (gameObjects[i] != NULL && gameObjects[i]->alive)
			{
				gameObjects[i]->Render();
			}
		}
	}
	if (toClear)
		gameObjects.clear();

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

void ModuleScene::CreateCube()
{
	GO_Cube* new_cube = new GO_Cube(1, 1, 1);
	gameObjects.push_back(new_cube);
}

void ModuleScene::CreateCylinder()
{
	GO_Cylinder* new_cyl = new GO_Cylinder(1, 5);
	gameObjects.push_back(new_cyl);
}

void ModuleScene::CreateSphere()
{
	GO_Sphere* new_sphere = new GO_Sphere(1);
	gameObjects.push_back(new_sphere);
}