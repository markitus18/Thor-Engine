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
#include "C_Camera.h"
#include "Intersections.h"

//#include <GLFW/glfw3.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	root = new GameObject(nullptr, "root");

	//TMP camera for testing purposes
	camera = new GameObject(root, "Camera");
	camera->GetComponent<C_Transform>()->SetPosition(float3(10, 10, 0));
	camera->CreateComponent(Component::Type::Camera);
	camera->GetComponent<C_Camera>()->Look(float3(0, 5, 0));
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

	root->Update();

	std::vector<GameObject*> gameObjects;
	TestGameObjectsCulling(gameObjects, root);

	for (uint i = 0; i < gameObjects.size(); i++)
	{
			gameObjects[i]->Draw(App->moduleEditor->shaded, App->moduleEditor->wireframe);
	}
	gameObjects.clear();
	camera->Draw(App->moduleEditor->shaded, App->moduleEditor->wireframe);
	//root->Draw(App->moduleEditor->shaded, App->moduleEditor->wireframe);

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

void ModuleScene::TestGameObjectsCulling(std::vector<GameObject*>& vector, GameObject* gameObject)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	if (mesh && Intersects(camera->GetComponent<C_Camera>()->frustum, mesh->GetGlobalAABB()))
	{
		vector.push_back(gameObject);
	}

	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		TestGameObjectsCulling(vector, gameObject->childs[i]);
	}
}
