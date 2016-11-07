#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Config.h"

#include <vector>



ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module("Camera", start_enabled)
{
	camera = new C_Camera(nullptr);

	camera->frustum.SetViewPlaneDistances(0.1f, 2000.0f);
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
bool ModuleCamera3D::Init(Config& config)
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

	App->renderer3D->camera = nullptr;
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
	return camera->frustum.Pos();
}

float3 ModuleCamera3D::GetReference() const
{
	return reference;
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

void ModuleCamera3D::SetNewTarget(const float3& new_target)
{
	float distance = reference.Distance(new_target);
	CenterOn(new_target, distance);
}

void ModuleCamera3D::Match(const C_Camera* camera)
{
	this->camera->Match(camera);
	reference = this->camera->frustum.Pos() + this->camera->frustum.Front() * 40;
}

void ModuleCamera3D::SetPosition(float3 position)
{
	float3 difference = position - camera->frustum.Pos();
	camera->frustum.SetPos(position);
	reference += difference;
}

// -----------------------------------------------------------------
C_Camera * ModuleCamera3D::GetCamera() const
{
	return camera;
}

void ModuleCamera3D::SaveScene(Config& root) const
{
	float3 position = camera->frustum.Pos();
	Config_Array pos = root.SetArray("Position");
	pos.AddFloat3(position);

	Config_Array ref = root.SetArray("Reference");
	ref.AddFloat3(reference);
}

void ModuleCamera3D::LoadScene(Config& root)
{
	camera->frustum.SetPos(root.GetArray("Position").GetFloat3(0));
	reference = root.GetArray("Reference").GetFloat3(0);
	Look(reference);
	camera->update_projection = true;
}

void ModuleCamera3D::Move_Keyboard(float dt)
{
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
		App->input->InfiniteHorizontal();
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
