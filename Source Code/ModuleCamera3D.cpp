#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include <vector>

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	camera = new C_Camera(NULL);

	camera->frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	camera->frustum.SetPerspective(1.0f, 1.0f);

	camera->frustum.SetPos(float3(10, 100, 0));
	camera->frustum.SetFront(float3::unitZ);
	camera->frustum.SetUp(float3::unitY);
	Look(float3(0, 0, 0));
	camera->update_projection = true;
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(camera);
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Init()
{
	App->renderer3D->camera = camera;

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	App->renderer3D->camera = NULL;
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (App->moduleEditor->UsingKeyboard() == false)
		Move_Keyboard(dt);

	if (App->moduleEditor->UsingMouse() == false)
		Move_Mouse();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
float3 ModuleCamera3D::GetPosition() const
{
	//return camera->frustum.;
	return float3(0, 0, 0);
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3& position)
{
	camera->Look(position);
	reference = position;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CenterOn(const float3& position, float distance)
{
	float3 v = camera->frustum.Front().Neg();
	camera->frustum.SetPos(position + (v * distance));
	reference = position;
}

// -----------------------------------------------------------------
C_Camera * ModuleCamera3D::GetCamera() const
{
	return camera;
}


void ModuleCamera3D::Move_Keyboard(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
		Zoom(1.0f);
}

void ModuleCamera3D::Move_Mouse()
{
	// Check motion for lookat / Orbit cameras
	int motion_x, motion_y;
	motion_x = -App->input->GetMouseXMotion();
	motion_y = App->input->GetMouseYMotion();

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && (motion_x != 0 || motion_y != 0))
	{
		Orbit(motion_x, -motion_y);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT && (motion_x != 0 || motion_y != 0))
	{
		//TODO: Kind of magic number. Consider other options?
		float distance = reference.Distance(camera->frustum.Pos());
		float3 Y_add = camera->frustum.Up() * motion_y * (distance / 1800);
		float3 X_add = camera->frustum.WorldRight() * motion_x * (distance / 1800);

		reference += X_add;
		reference += Y_add;

		camera->frustum.SetPos(camera->frustum.Pos() + X_add + Y_add);
	}
	// Mouse wheel for zoom
	int wheel = App->input->GetMouseZ();
	if (wheel != 0)
		Zoom(wheel);

	//// Mouse Picking
	//if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	//{
	//	GameObject* pick = Pick();
	//	if (pick != nullptr)
	//		App->editor->SetSelected(pick, (App->editor->selected == pick));
	//}
}

// -----------------------------------------------------------------
void ModuleCamera3D::Orbit(float dx, float dy)
{
	//Procedure: create a vector from camera position to reference position
	//Rotate that vector according to our mouse motion
	//Move the camera to where that vector ended up

	//TODO: this causes issues when rotating when frustrum Z = world +/- Y
	float3 vector = camera->frustum.Pos() - reference;

	Quat quat_y(camera->frustum.Up(), dx * 0.003);
	Quat quat_x(camera->frustum.WorldRight(), dy * 0.003);

	vector = quat_x.Transform(vector);
	vector = quat_y.Transform(vector);

	camera->frustum.SetPos(vector + reference);
	Look(reference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::Zoom(float zoom)
{
	float distance = reference.Distance(camera->frustum.Pos());
	vec newPos = camera->frustum.Pos() + camera->frustum.Front() * zoom * distance * 0.05f;
	camera->frustum.SetPos(newPos);
}
