#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
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

#pragma region vertices
	vertices = new vec3[

		//Bottom vertices
		(0, 0, 0), //0
		(0, 0, 1), //1
		(1, 0, 1), //2
		(1, 0, 0), //3

		//Upper vertices
		(0, 1, 0), //4
		(0, 1, 1), //5
		(1, 1, 1), //6
		(1, 1, 0) //7
	];

	index = new uint[0, 7, 3,
		0, 5, 7,

		0, 1, 5,
		1, 6, 5

	];
#pragma endregion

	glGenBuffers(1, (GLuint*) &(buf_id));
	glBindBuffer(GL_ARRAY_BUFFER, buf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36  * 3, vertices, GL_STATIC_DRAW);

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

	//Cube testing
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, buf_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);
	glDisableClientState(GL_VERTEX_ARRAY);




	bool toClear = false;
	for (int i = 0; i < Primitives.size(); i++)
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			toClear = true;
			delete (Primitives[i]);
			Primitives[i] = NULL;
		}
		else
		{
			if (Primitives[i] != NULL && Primitives[i]->alive)
			{
				Primitives[i]->Render();
			}
		}
	}
	if (toClear)
		Primitives.clear();

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
	Primitives.push_back(new_cube);
}

void ModuleScene::CreateCylinder()
{
	GO_Cylinder* new_cyl = new GO_Cylinder(1, 5);
	Primitives.push_back(new_cyl);
}

void ModuleScene::CreateSphere()
{
	GO_Sphere* new_sphere = new GO_Sphere(1);
	Primitives.push_back(new_sphere);
}