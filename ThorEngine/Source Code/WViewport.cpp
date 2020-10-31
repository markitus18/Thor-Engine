#include "WViewport.h"

#include "Engine.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Editor.h"
#include "M_Resources.h"
#include "M_SceneManager.h"

#include "Scene.h"
#include "GameObject.h"
#include "C_Camera.h"
#include "C_Transform.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

WViewport::WViewport(WindowFrame* parent, const char* name, ImGuiWindowClass* windowClass, int ID) : Window(parent, name, windowClass, ID), cameraReference(.0f, .0f, .0f)
{
	perspectiveCamera = new C_Camera(nullptr);
	perspectiveCamera->SetFarPlane(10000.0f);
	perspectiveCamera->frustum.SetPos(float3(-50, 50, -50));
	perspectiveCamera->Look(float3(.0f, .0f, .0f));
	perspectiveCamera->GenerateFrameBuffer();

	orthographicCamera = new C_Camera(nullptr);
	orthographicCamera->SetFarPlane(10000.0f);
	orthographicCamera->GenerateFrameBuffer();

	currentCamera = perspectiveCamera;
}

void WViewport::PrepareUpdate()
{
	//TODO: There is a one frame delay and it is lightly noticeable
	if (cameraSettingsNeedUpdate)
	{
		currentCamera->SetAspectRatio(textureSize.x / textureSize.y);
		currentCamera->SetResolution(textureSize.x, textureSize.y);
		cameraSettingsNeedUpdate = false;
	}
}

void WViewport::Draw()
{
	HandleInput();

	ImGui::SetNextWindowClass(windowClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	if (!ImGui::Begin(windowStrID.c_str(), &active, flags)) { ImGui::End(); return; }

	//TODO: Can we do it generically for all windows?
	Vec2 currentSize = Vec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	if (currentSize != windowSize || isTabBarHidden != ImGui::GetCurrentWindow()->DockNode->IsHiddenTabBar())
		OnResize(currentSize);

	DrawScene();
	DrawToolbarShared();
	DrawToolbarCustom();

	HandleGizmoUsage();

	LOG("Scene Texture Screen Position: %.0f x, %.0f y", sceneTextureScreenPosition.x, sceneTextureScreenPosition.y);
	LOG("Scene Texture Size: %.0f x, %.0f y", textureSize.x, textureSize.y);
	LOG("Gizmo Rect Origin: %.0f x, %.0f y", gizmoRectOrigin.x, gizmoRectOrigin.y);

	ImGui::End();
	ImGui::PopStyleVar();
}

void WViewport::DrawScene()
{
	//Top-left corner of the image in ImGui space coordinates (0y at the top of the screen)
	gizmoRectOrigin = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 25.0f); //<-- We need to subtract 25.0f which is the size of the header bar...

	//Bottom-left corner of the image in OpenGL space coordinates (0y at the bottom of the screen)
	sceneTextureScreenPosition = Vec2(gizmoRectOrigin.x, Engine->window->windowSize.y - gizmoRectOrigin.y - textureSize.y);

	ImGui::Image((ImTextureID)currentCamera->GetRenderTarget(), ImVec2(textureSize.x, textureSize.y), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE_6"))
		{
			if (payload->DataSize == sizeof(uint64))
			{
				uint64 resourceID = *(const uint64*)payload->Data;
				const ResourceBase& base = *Engine->moduleResources->GetResourceBase(resourceID);

				if (base.type == ResourceType::MODEL)
				{
					Engine->sceneManager->LoadModel(resourceID, scene);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	HandleGizmoUsage();

	if (ImGuizmo::IsUsing() == false)
		HandleInput();
}

void WViewport::DrawToolbarShared()
{

}

void WViewport::OnResize(Vec2 newSize)
{
	//Getting window size - some margins - separator (7)
	Window::OnResize(newSize);

	ImGuiContext& g = *ImGui::GetCurrentContext();

	float tabBarSize = 0;
	if (!isTabBarHidden)
		tabBarSize = g.FontSize + g.Style.FramePadding.y * 2.0f;

	textureSize.x = windowSize.x; //<-- -4 why??? It's not FramePadding nor WindowPadding...
	textureSize.y = windowSize.y - tabBarSize; //For some reason we need to subtract it again...

	cameraSettingsNeedUpdate = true;
}

//Converts a 2D point in the scene image to a 2D point in the real scene
Vec2 WViewport::ScreenToWorld(Vec2 p) const
{
	Vec2 ret = p - sceneTextureScreenPosition;
	ret = ret / windowSize * Engine->window->windowSize;
	return ret;
}

//Converts a 2D point in the real scene to a 2D point in the scene image
Vec2 WViewport::WorldToScreen(Vec2 p) const
{
	Vec2 ret = p / Engine->window->windowSize * windowSize;
	ret += sceneTextureScreenPosition;
	return ret;
}

void WViewport::SetScene(Scene* scene)
{
	if (this->scene != nullptr && this->scene != scene)
		scene->UnregisterViewport(this);

	this->scene = scene;
	scene->RegisterViewport(this);
}

void WViewport::CameraLookAt(const float3& position)
{
	currentCamera->Look(position);
	cameraReference = position;
}

// -----------------------------------------------------------------
void WViewport::CenterCameraOn(const float3& position, float distance)
{
	float3 v = currentCamera->frustum.Front().Neg();
	currentCamera->frustum.SetPos(position + (v * distance));
	cameraReference = position;
}

void WViewport::SetNewCameraTarget(const float3& new_target)
{
	float distance = cameraReference.Distance(new_target);
	CenterCameraOn(new_target, distance);
}

void WViewport::MatchCamera(const C_Camera* camera)
{
	currentCamera->Match(camera);
	cameraReference = currentCamera->frustum.Pos() + currentCamera->frustum.Front() * 40;
}

void WViewport::SetCameraPosition(float3 position)
{
	float3 difference = position - currentCamera->frustum.Pos();
	currentCamera->frustum.SetPos(position);
	cameraReference += difference;
}

void WViewport::OnClick(const Vec2& mousePos)
{
	float mouseNormX = mousePos.x / (float)Engine->window->windowSize.x;
	//TODO: quick fix, mouse click is inverting Y
	float mouseNormY = mousePos.y / (float)Engine->window->windowSize.y;

	//Normalizing mouse position in range of -1 / 1 // -1, -1 being at the bottom left corner
	mouseNormX = (mouseNormX - 0.5) / 0.5;
	mouseNormY = (mouseNormY - 0.5) / 0.5;

	//lastRay = Engine->renderer3D->camera->frustum.UnProjectLineSegment(mouseNormX, mouseNormY);

	//Engine->sceneManager->mainScene->PerformMousePick(lastRay);
}

void WViewport::MoveCamera_Mouse(float motion_x, float motion_y)
{
	// Mouse wheel for zoom
	int wheel = Engine->input->GetMouseZ();
	if (wheel != 0)
		ZoomCamera(wheel);
}

void WViewport::PanCamera(float motion_x, float motion_y)
{
	float distance = cameraReference.Distance(currentCamera->frustum.Pos());
	float3 Y_add = currentCamera->frustum.Up() * motion_y * (distance / 1800);
	float3 X_add = currentCamera->frustum.WorldRight() * -motion_x * (distance / 1800);

	cameraReference += X_add;
	cameraReference += Y_add;

	currentCamera->frustum.SetPos(currentCamera->frustum.Pos() + X_add + Y_add);
}

// -----------------------------------------------------------------
void WViewport::OrbitCamera(float dx, float dy)
{
	//Procedure: create a vector from camera position to reference position
	//Rotate that vector according to our mouse motion
	//Move the camera to where that vector ended up

	//TODO: this causes issues when rotating when frustum Z = world +/- Y
	float3 vector = currentCamera->frustum.Pos() - cameraReference;

	Quat quat_y(currentCamera->frustum.Up(), dx * 0.003);
	Quat quat_x(currentCamera->frustum.WorldRight(), dy * 0.003);

	vector = quat_x.Transform(vector);
	vector = quat_y.Transform(vector);

	currentCamera->frustum.SetPos(vector + cameraReference);
	CameraLookAt(cameraReference);
}

// -----------------------------------------------------------------
void WViewport::ZoomCamera(float zoom)
{
	float distance = cameraReference.Distance(currentCamera->frustum.Pos());
	vec newPos = currentCamera->frustum.Pos() + currentCamera->frustum.Front() * zoom * distance * 0.05f;
	currentCamera->frustum.SetPos(newPos);
}

//Handles user input
void WViewport::HandleInput()
{
	//if (Engine->moduleEditor->UsingKeyboard() == false)
	//	MoveCamera_Keyboard(Time::deltaTime);

	if (Engine->moduleEditor->UsingMouse() == false)
	{
		if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			//OnClick();
		}
		//Move_Mouse();
	}

	if (ImGui::IsItemHovered())
	{
		if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			Vec2 mousePos = ScreenToWorld(Vec2(Engine->input->GetMouseX(), Engine->window->windowSize.y - Engine->input->GetMouseY()));
			OnClick(mousePos);
		}
		MoveCamera_Mouse(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());

		draggingOrbit = Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT;
		draggingPan = Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT;

		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / windowSize * Engine->window->windowSize;

		MoveCamera_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
	}

	if (draggingOrbit)
	{
		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / windowSize * Engine->window->windowSize;

		MoveCamera_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		OrbitCamera(-mouseMotion_screen.x, -mouseMotion_screen.y);
		Engine->input->InfiniteHorizontal();

		if (Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE) draggingOrbit = false;
	}
	if (draggingPan)
	{
		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / windowSize * Engine->window->windowSize;

		MoveCamera_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		PanCamera(mouseMotion_screen.x, mouseMotion_screen.y);
		Engine->input->InfiniteHorizontal();

		if (Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE) draggingPan = false;
	}

	if (Engine->moduleEditor->UsingKeyboard() == false)
	{
		if (Engine->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (Engine->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (Engine->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
}

void WViewport::HandleGizmoUsage()
{
	if (Engine->moduleEditor->selectedGameObjects.size() <= 0) return;

	GameObject* gameObject = (GameObject*)Engine->moduleEditor->selectedGameObjects[0];

	float4x4 viewMatrix = currentCamera->frustum.ViewMatrix();
	viewMatrix.Transpose();
	float4x4 projectionMatrix = currentCamera->frustum.ProjectionMatrix().Transposed();
	float4x4 modelProjection = gameObject->GetComponent<C_Transform>()->GetGlobalTransform().Transposed();

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(gizmoRectOrigin.x, gizmoRectOrigin.y, textureSize.x, textureSize.y);

	float modelPtr[16];
	memcpy(modelPtr, modelProjection.ptr(), 16 * sizeof(float));
	ImGuizmo::MODE finalMode = (gizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : gizmoMode);
	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), gizmoOperation, finalMode, modelPtr);

	if (ImGuizmo::IsUsing())
	{
		float4x4 newMatrix;
		newMatrix.Set(modelPtr);
		modelProjection = newMatrix.Transposed();
		gameObject->GetComponent<C_Transform>()->SetGlobalTransform(modelProjection);
	}
}