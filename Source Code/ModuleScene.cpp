#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleImport.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"

#include "GameObject.h"
#include <gl/GL.h>
#include <gl/GLU.h>
//#include <stdio.h>

//#include <GLFW/glfw3.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	root = new GameObject(NULL, "root");
}

ModuleScene::~ModuleScene()
{
}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");



	bool ret = true;

	timer.Start();
	timer.Stop();
	reset = false;
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading scene");

	RELEASE(root);

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	int mouseX = App->input->GetMouseX();
	int mouseY = App->input->GetMouseY();

	if (mouseX > App->renderer3D->window_width - 10)
	{
		App->input->SetMouseX(10);
	}
	if (mouseX < 10)
	{
		App->input->SetMouseX(App->renderer3D->window_width - 10);
	}

	if (mouseY > App->renderer3D->window_height - 10)
	{
		App->input->SetMouseY(10);
	}
	if (mouseY < 10)
	{
		App->input->SetMouseY(App->renderer3D->window_height - 10);
	}




	if (drawGrid)
	{
		//TODO: Move this into a mesh "prefab"
		glLineWidth(1.0f);
		
		glBegin(GL_LINES);
		
		float d = 20.0f;
		
		for(float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}
		
		glEnd();
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		drawGrid = !drawGrid;
	}

	root->Draw(App->moduleEditor->shaded, App->moduleEditor->wireframe);

	//LOG("GameObjects in scene: %i", tmp_goCount)
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

GameObject* ModuleScene::getRoot()
{
	return root;
}
const GameObject* ModuleScene::getRoot() const
{
	return root;
}