#include "Engine.h"
#include "M_Camera3D.h"
#include "M_Editor.h"
#include "M_Input.h"
#include "M_Window.h"
#include "C_Camera.h"
#include "M_Renderer3D.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "M_SceneManager.h"
#include "Scene.h"
#include "Vec2.h"
#include "Time.h"

#include <vector>
#include <map>

//TODO: Camera editor movement should NOT be in a module! Viewport window will handle this

M_Camera3D::M_Camera3D(bool start_enabled) : Module("Camera", start_enabled)
{
	camera = new C_Camera(nullptr);

	camera->frustum.SetPos(float3(10, 100, 0));
	camera->frustum.SetFront(float3::unitZ);
	camera->frustum.SetUp(float3::unitY);
}

M_Camera3D::~M_Camera3D()
{
	RELEASE(camera);
}

// -----------------------------------------------------------------
bool M_Camera3D::Init(Config& config)
{
	//Engine->renderer3D->camera = camera;

	return true;
}

// -----------------------------------------------------------------
bool M_Camera3D::Start()
{
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool M_Camera3D::CleanUp()
{
	LOG("Cleaning camera");

	//Engine->renderer3D->camera = nullptr;
	return true;
}

// -----------------------------------------------------------------
update_status M_Camera3D::Update()
{
	if (Engine->moduleEditor->UsingMouse() == false)
	{
		if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			//OnClick();
		}
		//Move_Mouse();
	}
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
float3 M_Camera3D::GetPosition() const
{
	return camera->frustum.Pos();
}

float3 M_Camera3D::GetReference() const
{
	return reference;
}

void M_Camera3D::DrawRay()
{
	glColor4f(1, 0, 0, 1);

	//Between-planes right
	GLfloat pointA[3] = { lastRay.a.x, lastRay.a.y, lastRay.a.z };
	GLfloat pointB[3] = { lastRay.b.x, lastRay.b.y, lastRay.b.z };

	glBegin(GL_LINES);
	glVertex3fv(pointA);
	glVertex3fv(pointB);
	glEnd();

	glColor4f(1, 1, 1, 1);
}
