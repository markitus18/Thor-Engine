#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include <vector>

ModuleCamera::ModuleCamera(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	camera = new C_Camera(NULL);

	camera->frustum.pos = float3::zero;
	camera->frustum.front = float3::unitZ;
	camera->frustum.up = float3::unitY;
	
	camera->frustum.nearPlaneDistance = 0.1f;
	camera->frustum.farPlaneDistance = 1000.0f;

	camera->frustum.horizontalFov = 1.0f;
	camera->frustum.verticalFov = 1.0f;

	camera->update_projection = true;
}

ModuleCamera::~ModuleCamera()
{
	RELEASE(camera);
}

// -----------------------------------------------------------------
bool ModuleCamera::Init()
{
	App->renderer3D->camera = camera;

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera::CleanUp()
{
	LOG("Cleaning camera");

	App->renderer3D->camera = NULL;
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera::Update(float dt)
{
	if (App->moduleEditor->UsingKeyboard() == false)
		Move_Keyboard(dt);

	if (App->moduleEditor->UsingMouse() == false)
		Move_Mouse();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
float3 ModuleCamera::GetPosition() const
{
	return camera->frustum.pos;
}

// -----------------------------------------------------------------
void ModuleCamera::Look(const float3& position)
{
	camera->Look(position);
}

// -----------------------------------------------------------------
void ModuleCamera::CenterOn(const float3& position, float distance)
{
	float3 v = camera->frustum.front.Neg();
	camera->frustum.pos = position + (v * distance);
	reference = position;
}

// -----------------------------------------------------------------
C_Camera * ModuleCamera::GetCamera() const
{
	return camera;
}


void ModuleCamera::Move_Keyboard(float dt)
{

}

void ModuleCamera::Move_Mouse()
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
void ModuleCamera::Orbit(float dx, float dy)
{
	float3 point = reference;

	float3 focus = camera->frustum.pos - point;

	Quat quat_y(camera->frustum.up, dx);
	Quat quat_x(camera->frustum.WorldRight(), dy);

	focus = quat_x.Transform(focus);
	focus = quat_y.Transform(focus);

	camera->frustum.pos = focus + point;

	Look(point);
}

// -----------------------------------------------------------------
void ModuleCamera::Zoom(float zoom)
{
	float distance = reference.Distance(camera->frustum.pos);
	camera->frustum.pos += camera->frustum.front * zoom * distance * 0.05;
}
