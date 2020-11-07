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
	GameObject* perspectiveGO = new GameObject();
	perspectiveCamera = new C_Camera(nullptr);
	perspectiveGO->AddComponent(perspectiveCamera);

	perspectiveCamera->SetFarPlane(10000.0f);
	perspectiveGO->GetComponent<C_Transform>()->SetPosition(float3(-100.0f, 50.0f, -50.0f));
	perspectiveGO->GetComponent<C_Transform>()->LookAt(float3::zero);
	perspectiveGO->Update(.0f);

	perspectiveCamera->GenerateFrameBuffer();
	
	GameObject* orthographicGO = new GameObject();
	orthographicCamera = new C_Camera(nullptr);
	orthographicGO->AddComponent(orthographicCamera);

	orthographicCamera->SetFarPlane(10000.0f);
	orthographicGO->GetComponent<C_Transform>()->SetPosition(float3(0.0f, 50.f, 0.0f));
	orthographicGO->GetComponent<C_Transform>()->LookAt(float3::zero);
	orthographicGO->Update(.0f);

	orthographicCamera->GenerateFrameBuffer();
	
	currentCamera = perspectiveCamera;
	ImGuizmo::Enable(true);
}

WViewport::~WViewport()
{
	RELEASE(perspectiveCamera->gameObject);
	RELEASE(orthographicCamera->gameObject);
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
	ImGui::SetNextWindowClass(windowClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	if (!ImGui::Begin(windowStrID.c_str(), &active, flags)) { ImGui::End(); return; }

	//TODO: Can we check for resizing generically for all windows?
	Vec2 currentSize = Vec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	if (currentSize != windowSize || isTabBarHidden != (!ImGui::GetCurrentWindow()->DockNode || ImGui::GetCurrentWindow()->DockNode->IsHiddenTabBar()))
		OnResize(currentSize);

	DrawScene();
	DrawToolbarShared();
	DrawToolbarCustom();

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

	textureSize.x = windowSize.x - 1; //<-- -4 why??? It's not FramePadding nor WindowPadding...
	textureSize.y = windowSize.y - tabBarSize - 1; //For some reason we need to subtract it again...

	cameraSettingsNeedUpdate = true;
}

//Converts a 2D point in the scene image to a 2D point in the real scene
Vec2 WViewport::ScreenToWorld(Vec2 p) const
{
	Vec2 ret = p - sceneTextureScreenPosition;
	ret = ret / windowSize * Vec2(currentCamera->resolution.x, currentCamera->resolution.y);
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

// -----------------------------------------------------------------
void WViewport::FocusCameraOnPosition(const float3& target, float distance)
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float3 v = transform->GetFwd().Neg();

	transform->SetPosition(target + (v * distance));
	currentCamera->gameObject->Update(.0f);

	cameraReference = target;
}

void WViewport::SetNewCameraTarget(const float3& new_target)
{
	float distance = cameraReference.Distance(new_target);
	FocusCameraOnPosition(new_target, distance);
}

void WViewport::MatchCamera(const C_Camera* camera)
{
	C_Transform* targetTransform = camera->gameObject->GetComponent<C_Transform>();
	C_Transform* cameraTransform = currentCamera->gameObject->GetComponent<C_Transform>();
	cameraTransform->SetGlobalTransform(targetTransform->GetTransform());
	currentCamera->gameObject->Update(.0f);

	//TODO: Store reference in C_Camera (?)
	cameraReference = cameraTransform->GetPosition() + cameraTransform->GetFwd() * 40;
}

void WViewport::SetCameraPosition(float3 position)
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float3 offset = position - transform->GetPosition();
	transform->SetPosition(position);  //TODO: Use Global Position
	currentCamera->gameObject->Update(.0f);

	cameraReference += offset;
}

void WViewport::OnClick(const Vec2& mousePos)
{
	float mouseNormX = mousePos.x / currentCamera->resolution.x;
	float mouseNormY = mousePos.y / currentCamera->resolution.y;

	//Normalizing mouse position in range of -1 / 1 // -1, -1 being at the bottom left corner
	mouseNormX = (mouseNormX - 0.5) / 0.5;
	mouseNormY = (mouseNormY - 0.5) / 0.5;

	LineSegment ray = currentCamera->GenerateRaycast(mouseNormX, mouseNormY);
	scene->PerformMousePick(ray);
}

void WViewport::PanCamera(float motion_x, float motion_y)
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float distance = cameraReference.Distance(transform->GetPosition());

	float3 deltaX = transform->GetRight() * motion_x * (distance / 1800);
	float3 deltaY = transform->GetUp() * motion_y * (distance / 1800);

	cameraReference += deltaX;
	cameraReference += deltaY;

	transform->SetPosition(transform->GetPosition() + deltaX + deltaY);
	currentCamera->gameObject->Update(.0f);
}

// -----------------------------------------------------------------
void WViewport::OrbitCamera(float dx, float dy)
{
	// Create a vector from camera position to reference position
	// Rotate that vector according to our mouse motion
	// Move the camera to where that vector ended up

	//TODO: When the camera forward is near (0, -1, 0) the rotation starts tittering
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float3 vector = transform->GetPosition() - cameraReference;

	Quat quat_y(transform->GetUp(), dx * 0.003);
	Quat quat_x(transform->GetRight().Neg(), dy * 0.003);

	vector = quat_x.Transform(vector);
	vector = quat_y.Transform(vector);

	//Change camera position
	transform->SetPosition(vector + cameraReference); //TODO: Use Global Position

	//Center camera back to the reference
	currentCamera->gameObject->GetComponent<C_Transform>()->LookAt(cameraReference);
	currentCamera->gameObject->Update(.0f);
}

void WViewport::TurnCamera(float motion_x, float motion_y)
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();

	// Yaw Rotation: We rotate around world up (0, 1, 0) to keep the Right axis of the camera always parallel to the ground
	float3x3 yawRotation = float3x3::RotateAxisAngle(float3::unitY, motion_x);
	float3 newRight = yawRotation * transform->GetRight();

	// Pitch Rotation: We rotate our fwd and up along our current Right axis
	float3x3 pitchRotation = float3x3::RotateAxisAngle(newRight, motion_y);
	float3 newUp = pitchRotation * yawRotation * transform->GetUp();
	float3 newFwd = pitchRotation * yawRotation * transform->GetFwd();

	// Fix-up to lock the camera's front angle to not go over the world's up
	if (newUp.y < 0.0f)
	{
		newFwd = float3(0.0f, transform->GetFwd().y > 0.0f ? 1.0f : -1.0f, 0.0f);
		newUp = newFwd.Cross(newRight);
	}
	transform->SetRotationAxis(newRight, newUp, newFwd);
	currentCamera->gameObject->Update(.0f);

	// Moving the camera reference to the same distance we had before
	float distancetoReference = (cameraReference - transform->GetPosition()).Length();
	cameraReference = transform->GetPosition() + transform->GetFwd() * distancetoReference;
}

void WViewport::ZoomCamera(float zoom)
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float distance = cameraReference.Distance(transform->GetPosition());
	vec newPos = transform->GetPosition() + transform->GetFwd() * zoom * distance * 0.05f;
	transform->SetPosition(newPos);

	currentCamera->gameObject->Update(.0f);
}

//Handles user input
void WViewport::HandleInput()
{
	if (ImGui::IsItemHovered())
	{
		if (int wheel = Engine->input->GetMouseZ())
			ZoomCamera(wheel);
		
		if (Engine->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && Engine->moduleEditor->selectedGameObjects.size() > 0)
		{
			float distance = (cameraReference - currentCamera->gameObject->GetComponent<C_Transform>()->GetPosition()).Length();
			GameObject* gameObject = ((GameObject*)Engine->moduleEditor->selectedGameObjects[0]);
			FocusCameraOnPosition(gameObject->GetComponent<C_Transform>()->GetPosition(), distance);
		}
		
		if (Engine->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN && cameraInputOperation == CameraInputOperation::NONE)
		{
			cameraInputOperation = CameraInputOperation::ORBIT;
		}
		if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && cameraInputOperation == CameraInputOperation::NONE)
		{
			cameraInputOperation = CameraInputOperation::SELECTION;
			selectionStartPoint = Vec2(Engine->input->GetMouseY(), Engine->input->GetMouseY());
		}
		if (Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN && cameraInputOperation == CameraInputOperation::NONE)
		{
			cameraInputOperation = CameraInputOperation::TURN;
		}
		if (Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN && cameraInputOperation == CameraInputOperation::NONE)
		{
			cameraInputOperation = CameraInputOperation::PAN;
		}
	}

	switch (cameraInputOperation)
	{
		case(CameraInputOperation::NONE): break;

		case(CameraInputOperation::SELECTION):
		{
			// TODO: Keep track of mouse movement, generate a box at frustum projection, select multiple objects

			// End selection on mouse button release
			if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				Vec2 mousePos = ScreenToWorld(Vec2(Engine->input->GetMouseX(), Engine->window->windowSize.y - Engine->input->GetMouseY()));
				OnClick(mousePos);
				cameraInputOperation = CameraInputOperation::NONE;
			}
			break;
		}
		case(CameraInputOperation::ORBIT):
		{
			if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
				Vec2 mouseMotion_screen = mouseMotion * 1.5f;// / windowSize * Engine->window->windowSize;

				OrbitCamera(-mouseMotion_screen.x, -mouseMotion_screen.y);
				Engine->input->InfiniteHorizontal();
			}
			if (Engine->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
				cameraInputOperation = CameraInputOperation::NONE;
			break;
		}
		case(CameraInputOperation::PAN):
		{
			Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
			Vec2 mouseMotion_screen = mouseMotion * 1.7f;// / windowSize * Engine->window->windowSize;

			PanCamera(mouseMotion_screen.x, mouseMotion_screen.y);
			Engine->input->InfiniteHorizontal();

			if (Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE)
				cameraInputOperation = CameraInputOperation::NONE;
			break;
		}
		case(CameraInputOperation::TURN):
		{
			Vec2 mouseMotion = Vec2(-Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
			Vec2 mouseMotion_screen = mouseMotion / 350.0f;// / windowSize * Engine->window->windowSize;

			TurnCamera(mouseMotion_screen.x, mouseMotion_screen.y);
			Engine->input->InfiniteHorizontal();
			HandleKeyboardInput();

			if (Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)
				cameraInputOperation = CameraInputOperation::NONE;
			break;
		}
	}

	if (Engine->moduleEditor->UsingKeyboard() == false) //TODO: Only when the frame has focus
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

	float4x4 viewMatrix = currentCamera->GetOpenGLViewMatrix();
	float4x4 projectionMatrix = currentCamera->GetOpenGLProjectionMatrix();
	float4x4 modelProjection = gameObject->GetComponent<C_Transform>()->GetTransform().Transposed();

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

void WViewport::HandleKeyboardInput()
{
	C_Transform* transform = currentCamera->gameObject->GetComponent<C_Transform>();
	float3 deltaRight = float3::zero, deltaUp = float3::zero, deltaFwd = float3::zero;

	if (Engine->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		deltaFwd = transform->GetFwd() * 1.0f;
	}
	if (Engine->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		deltaFwd = transform->GetFwd().Neg() * 1.0f;
	}

	if (Engine->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		deltaRight = transform->GetRight() * 1.0f;
	}
	if (Engine->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		deltaRight = transform->GetRight().Neg() * 1.0f;
	}

	if (Engine->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		deltaUp = float3::unitY * 1.0f;
	}
	if (Engine->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		deltaUp = float3::unitY.Neg() * 1.0f;
	}

	transform->SetPosition(transform->GetPosition() + deltaFwd + deltaRight + deltaUp);
	cameraReference += deltaRight + deltaUp;
}