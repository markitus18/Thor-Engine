#include "W_Scene.h"

#include "ImGui\imgui.h"
#include "Glew/include/glew.h"

#include "Dock.h"

#include "Application.h"
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

W_Scene::W_Scene(M_Editor* editor) : DWindow(editor, "Scene")
{
	allowScrollbar = false;
	ImGuizmo::Enable(true);
}

void W_Scene::Draw()
{
	ImGui::SetCursorPos(ImVec2(img_offset.x, img_offset.y));
	img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) + Vec2(0, img_size.y);
	img_corner.y = App->window->windowSize.y - img_corner.y; //ImGui 0y is on top so we need to convert 0y on botton

	ImGui::Image((ImTextureID)App->renderer3D->renderTexture, ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE_6"))
		{
			if (payload->DataSize == sizeof(uint64))
			{
				uint64 resourceID = *(const uint64*)payload->Data;
				Resource* resource = App->moduleResources->GetResource(resourceID);

				if (resource->GetType() == Resource::Type::PREFAB)
				{
					App->moduleResources->LoadPrefab(resourceID);
				}

			}
		}
		ImGui::EndDragDropTarget();
	}

	HandleGizmoUsage();

	if (ImGuizmo::IsUsing() == false)
		HandleInput();
}

void W_Scene::OnResize()
{
	//Getting window size - some margins - separator (7)
	win_size = Vec2(parent->size.x, parent->size.y) - Vec2(17, 25 + 10); //TODO: should be using tab spacing (25) but at the beggining it has not been calculated

	//Calculating the image size according to the window size.
	img_size = App->window->windowSize;// -Vec2(0.0f, 25.0f); //Removing the tab area
	if (img_size.x > win_size.x)
	{
		img_size /= (img_size.x / win_size.x);
	}
	if (img_size.y > win_size.y)
	{
		img_size /= (img_size.y / win_size.y);
	}
	img_offset = Vec2(win_size.x - img_size.x, win_size.y - img_size.y) / 2 + Vec2(12.0f, 5.0f);

}

//Converts a 2D point in the scene image to a 2D point in the real scene
Vec2 W_Scene::ScreenToWorld(Vec2 p) const
{
	Vec2 ret = p - img_corner;
	ret = ret / img_size * App->window->windowSize;
	return ret;
}

//Converts a 2D point in the real scene to a 2D point in the scene image
Vec2 W_Scene::WorldToScreen(Vec2 p) const
{
	Vec2 ret = p / App->window->windowSize * img_size;
	ret += img_corner;
	return ret;
}

//Handles user input
void W_Scene::HandleInput()
{
	if (ImGui::IsItemHovered())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			Vec2 mousePos = ScreenToWorld(Vec2(App->input->GetMouseX(), App->window->windowSize.y - App->input->GetMouseY()));
			App->camera->OnClick(mousePos);
		}
		draggingOrbit = App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT;
		draggingPan = App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT;

		Vec2 mouseMotion = Vec2(App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * App->window->windowSize;

		App->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
	}
	if (draggingOrbit)
	{
		Vec2 mouseMotion = Vec2(App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * App->window->windowSize;

		App->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		App->camera->Orbit(-mouseMotion_screen.x, -mouseMotion_screen.y);
		App->input->InfiniteHorizontal();

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE) draggingOrbit = false;
	}
	if (draggingPan)
	{
		Vec2 mouseMotion = Vec2(App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
		Vec2 mouseMotion_screen = mouseMotion / img_size * App->window->windowSize;

		App->camera->Move_Mouse(mouseMotion_screen.x, mouseMotion_screen.y);
		App->camera->Pan(mouseMotion_screen.x, mouseMotion_screen.y);
		App->input->InfiniteHorizontal();

		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE) draggingPan = false;
	}

	if (editor->UsingKeyboard() == false)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
}

void W_Scene::HandleGizmoUsage()
{
	if (App->moduleEditor->selectedGameObjects.size() <= 0) return;

	GameObject* gameObject = (GameObject*)App->moduleEditor->selectedGameObjects[0];

	float4x4 viewMatrix = App->camera->GetCamera()->frustum.ViewMatrix();
	viewMatrix.Transpose();
	float4x4 projectionMatrix = App->camera->GetCamera()->frustum.ProjectionMatrix().Transposed();
	float4x4 modelProjection = gameObject->GetComponent<C_Transform>()->GetGlobalTransform().Transposed();

	ImGuizmo::SetDrawlist();
	cornerPos = Vec2(img_corner.x, App->window->windowSize.y - img_corner.y - img_size.y);
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