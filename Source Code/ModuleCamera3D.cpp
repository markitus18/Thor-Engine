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

	camera->frustum.SetPos(float3(10, 50, 0));
	camera->frustum.SetFront(float3::unitZ);
	camera->frustum.SetUp(float3::unitY);

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
	//if (App->moduleEditor->UsingKeyboard() == false)
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
}

// -----------------------------------------------------------------
void ModuleCamera3D::CenterOn(const float3& position, float distance)
{
	//float3 v = camera->frustum.front.Neg();
	//camera->frustum.pos = position + (v * distance);
	//reference = position;
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
	//// Check motion for lookat / Orbit cameras
	//int motion_x, motion_y;
	//motion_x = App->input->GetMouseXMotion;
	//motion_y = App->input->GetMouseYMotion; 
	//GetMouseMotion(motion_x, motion_y);

	//if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && (motion_x != 0 || motion_y != 0))
	//{
	//	float dx = (float)-motion_x * rot_speed * dt;
	//	float dy = (float)-motion_y * rot_speed * dt;

	//	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	//		Orbit(dx, dy);
	//	else
	//		LookAt(dx, dy);
	//}

	//// Mouse wheel for zoom
	//int wheel = App->input->GetMouseWheel();
	//if (wheel != 0)
	//	Zoom(wheel * zoom_speed * dt);

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
	//float3 point = reference;

	//float3 focus = camera->frustum.pos - point;

	//Quat quat_y(camera->frustum.up, dx);
	//Quat quat_x(camera->frustum.WorldRight(), dy);

	//focus = quat_x.Transform(focus);
	//focus = quat_y.Transform(focus);

	//camera->frustum.pos = focus + point;

	//Look(point);
}

// -----------------------------------------------------------------
void ModuleCamera3D::Zoom(float zoom)
{
	float distance = reference.Distance(camera->frustum.Pos());
	camera->frustum.Translate(camera->frustum.Front());
}
