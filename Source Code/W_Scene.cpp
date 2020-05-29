#include "W_Scene.h"

#include "ImGui\imgui.h"
#include "Glew/include/glew.h"

#include "Engine.h"
#include "M_Window.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_Input.h"
#include "M_Editor.h"
#include "M_Resources.h"

#include "C_Camera.h"
#include "TreeNode.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "Component.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

W_Scene::W_Scene(M_Editor* editor, ImGuiWindowClass* windowClass) : Window(editor, GetName(), windowClass)
{
	ImGuizmo::Enable(true);
}

void W_Scene::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin("Scene", &active)) { ImGui::End(); return; }

	ImVec2 winSize = ImGui::GetWindowSize();
	if (winSize.x != windowSize.x || winSize.y != windowSize.y)
		OnResize(Vec2(winSize.x, winSize.y));

	ImGui::SetCursorPos(/*ImGui::GetCursorPos() +*/ ImVec2(img_offset.x, img_offset.y));
		img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) + Vec2(0, img_size.y);
		img_corner.y = Engine->window->windowSize.y - img_corner.y; //ImGui 0y is on top so we need to convert 0y on botton

	ImGui::Image((ImTextureID)Engine->renderer3D->renderTexture, ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE_6"))
		{
			if (payload->DataSize == sizeof(uint64))
			{
				uint64 resourceID = *(const uint64*)payload->Data;
				Resource* resource = Engine->moduleResources->GetResource(resourceID);

				if (resource->GetType() == Resource::Type::PREFAB)
				{
					Engine->moduleResources->LoadModel(resourceID);
				}

			}
		}
		ImGui::EndDragDropTarget();
	}

	HandleGizmoUsage();

	if (ImGuizmo::IsUsing() == false)
		HandleInput();

	ImGui::End();
}

void W_Scene::OnResize(Vec2 newSize)
{
	//Getting window size - some margins - separator (7)
	win_size = newSize;

	//Calculating the image size according to the window size.
	img_size = Engine->window->windowSize;// -Vec2(0.0f, 25.0f); //Removing the tab area
	if (img_size.x > win_size.x - 10.0f)
	{
		img_size /= (img_size.x / (win_size.x - 10.0f));
	}
	if (img_size.y > win_size.y - 10.0f)
	{
		img_size /= (img_size.y / (win_size.y - 10.0f));
	}
	img_offset = Vec2(win_size.x - 5.0f - img_size.x, win_size.y - 5.0f - img_size.y) / 2;

}

//Converts a 2D point in the scene image to a 2D point in the real scene
Vec2 W_Scene::ScreenToWorld(Vec2 p) const
{
	Vec2 ret = p - img_corner;
	ret = ret / img_size * Engine->window->windowSize;
	return ret;
}

//Converts a 2D point in the real scene to a 2D point in the scene image
Vec2 W_Scene::WorldToScreen(Vec2 p) const
{
	Vec2 ret = p / Engine->window->windowSize * img_size;
	ret += img_corner;
	return ret;
}

//Handles user input
void W_Scene::HandleInput()
{
	if (ImGui::IsItemHovered())
	{
		if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			Vec2 mousePos = ScreenToWorld(Vec2(Engine->input->GetMouseX(), Engine->window->windowSize.y - Engine->input->GetMouseY()));
			Engine->camera->OnClick(mousePos);
		}
		draggingOrbit = Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT;
		draggingPan = Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT;

		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * Engine->window->windowSize;

		Engine->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
	}
	if (draggingOrbit)
	{
		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * Engine->window->windowSize;

		Engine->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		Engine->camera->Orbit(-mouseMotion_screen.x, -mouseMotion_screen.y);
		Engine->input->InfiniteHorizontal();

		if (Engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE) draggingOrbit = false;
	}
	if (draggingPan)
	{
		Vec2 mouseMotion = Vec2(Engine->input->GetMouseXMotion(), Engine->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * Engine->window->windowSize;

		Engine->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		Engine->camera->Pan(mouseMotion_screen.x, mouseMotion_screen.y);
		Engine->input->InfiniteHorizontal();

		if (Engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE) draggingPan = false;
	}

	if (editor->UsingKeyboard() == false)
	{
		if (Engine->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (Engine->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (Engine->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
}

void W_Scene::HandleGizmoUsage()
{
	if (Engine->moduleEditor->selectedGameObjects.size() <= 0) return;

	GameObject* gameObject = (GameObject*)Engine->moduleEditor->selectedGameObjects[0];

	float4x4 viewMatrix = Engine->camera->GetCamera()->frustum.ViewMatrix();
	viewMatrix.Transpose();
	float4x4 projectionMatrix = Engine->camera->GetCamera()->frustum.ProjectionMatrix().Transposed();
	float4x4 modelProjection = gameObject->GetComponent<C_Transform>()->GetGlobalTransform().Transposed();

	ImGuizmo::SetDrawlist();
	cornerPos = Vec2(img_corner.x, Engine->window->windowSize.y - img_corner.y - img_size.y);
	ImGuizmo::SetRect(img_corner.x, cornerPos.y, img_size.x, img_size.y);

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