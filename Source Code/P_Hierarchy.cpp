#include "P_Hierarchy.h"
#include "Application.h"
#include "M_Scene.h"
#include "GameObject.h"
#include "M_Camera3D.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Mesh.h"	
#include "C_Material.h"

#include "ImGui\imgui.h"

#include "Application.h"
#include "M_Editor.h"
#include "M_Input.h"

P_Hierarchy::P_Hierarchy()
{
}

P_Hierarchy::~P_Hierarchy()
{

}

void P_Hierarchy::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		ImGui::Begin("Hierarchy", &active, flags);
		windowFocused = ImGui::GetWindowIsFocused();
		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawRootChilds(App->scene->GetRoot(), default_flags);
		ImGui::End();

		switch (shiftSelectionStage)
		{
			case(SELECT_START): shiftSelectionStage = SELECTION_LOOP; break;
		}

		if ((App->input->GetKey(SDL_SCANCODE_DOWN)) == KEY_DOWN)
		{
			GameObject* next = GetNextHierarchyNode(App->moduleEditor->lastSelected);
			if (next != nullptr && next != App->scene->GetRoot())
			{
				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
					App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
				{
					GameObject* previous = GetPreviousHierarchyNode(App->moduleEditor->lastSelected);
					if (previous == nullptr || (previous != nullptr && previous->IsSelected() == true))
					{
						App->moduleEditor->AddSelect(next);
					}
					else if (next->IsSelected() == true)
					{
						App->moduleEditor->UnselectSingle(App->moduleEditor->lastSelected);
					}
					else
					{
						App->moduleEditor->AddSelect(next);
					}
				}
				else
				{
					App->moduleEditor->SelectSingle(next);
				}
				App->moduleEditor->lastSelected = next;
			}
		}

		if ((App->input->GetKey(SDL_SCANCODE_UP)) == KEY_DOWN)
		{
			GameObject* previous = GetPreviousHierarchyNode(App->moduleEditor->lastSelected);
			if (previous != nullptr && previous != App->scene->GetRoot())
			{
				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
					App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
				{
					GameObject* next = GetNextHierarchyNode(App->moduleEditor->lastSelected);
					//Bottom end of hierarchy -> next = nullptr
					if ((next != nullptr && next->IsSelected() == true))
					{
						App->moduleEditor->AddSelect(previous);
					}
					else if (previous->IsSelected() == true)
					{
						App->moduleEditor->UnselectSingle(App->moduleEditor->lastSelected);
					}
					else
					{
						App->moduleEditor->AddSelect(previous);
					}
				}
				else
				{
					App->moduleEditor->SelectSingle(previous);
				}
				App->moduleEditor->lastSelected = previous;
			}
		}
	}
}

void P_Hierarchy::DrawRootChilds(GameObject* gameObject, ImGuiTreeNodeFlags default_flags)
{
	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		DrawGameObject(gameObject->childs[i], default_flags);
	}
}

void P_Hierarchy::DrawGameObject(GameObject* gameObject, ImGuiTreeNodeFlags default_flags)
{

	DisplayGameObjectNode(gameObject, default_flags);

	if (shiftSelectionStage != NONE)
	{
		HandleShiftSelection(gameObject);
	}

	HandleUserInput(gameObject);

	if (ImGui::BeginPopup(gameObject->name.c_str()))
	{
		if (ImGui::Button("delete"))
		{
			App->scene->DeleteGameObject(gameObject);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (gameObject->hierarchyOpen)
	{
		for (uint i = 0; i < gameObject->childs.size(); i++)
		{
			DrawGameObject(gameObject->childs[i], default_flags);
		}
		if (gameObject->hierarchyOpen)
			ImGui::TreePop();
	}
	//In case GameObject had been selected in the last frame, since we opened tree node, reset the selection flag
	if (gameObject->beenSelected == true)
		gameObject->beenSelected = false;
}

void P_Hierarchy::DisplayGameObjectNode(GameObject* gameObject, ImGuiTreeNodeFlags defaultFlags)
{
	ImGuiTreeNodeFlags gameObject_flag = defaultFlags;
	if (gameObject->childs.empty())
	{
		gameObject_flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (gameObject->IsSelected())
		gameObject_flag |= ImGuiTreeNodeFlags_Selected;

	if (gameObject->IsParentActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.4));

	if (gameObject->beenSelected == true)
	{
		ImGui::SetNextTreeNodeOpen(true);
	}

	bool nodeOpen = ImGui::TreeNodeEx(gameObject, gameObject_flag, gameObject->name.c_str());
	gameObject->hierarchyOpen = gameObject->childs.empty() ? false : nodeOpen;

	if (gameObject->IsParentActive() == false)
		ImGui::PopStyleColor();
}

void P_Hierarchy::HandleUserInput(GameObject* gameObject)
{
	if (ImGui::IsItemClicked())
	{
		//If control is pressed, select / unselect only the GameObject
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			if (gameObject->IsSelected())
			{
				App->moduleEditor->UnselectSingle(gameObject);
			}
			else
			{
				App->moduleEditor->AddSelect(gameObject);
				App->moduleEditor->lastSelected = gameObject;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
		{
			if (App->moduleEditor->selectedGameObjects.size() == 0)
				App->moduleEditor->SelectSingle(gameObject);
			else
			{
				shiftSelectionStage = SELECT_START;
				shiftClickedGO = gameObject;
				shiftSelects = !gameObject->IsSelected();
			}
		}
		//If not ctrl pressed or shift, unselect all other GameObjects
		else
		{
			App->moduleEditor->SelectSingle(gameObject);
		}
	}

	if (ImGui::IsItemHovered())
	{
		if (ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup(gameObject->name.c_str());
			LOG("Opening popup");
		}


		if (ImGui::IsMouseDoubleClicked(0))
		{
			float3 pos = gameObject->GetComponent<C_Transform>()->GetGlobalPosition();
			App->camera->SetNewTarget(vec(pos.x, pos.y, pos.z));
			LOG("New camera look position: %f x, %f y, %f z", pos.x, pos.y, pos.z);
		}

	}
}

void P_Hierarchy::HandleShiftSelection(GameObject* gameObject)
{
	bool begunSelection = false;
	switch (shiftSelectionStage)
	{
		case(SELECTION_LOOP):
		{
			if (gameObject == App->moduleEditor->lastSelected || gameObject == shiftClickedGO)
			{
				shiftSelectionStage = SELECTING;
				if (App->moduleEditor->lastSelected != shiftClickedGO)
					begunSelection = true;
			}
			else
				break;
		}
		case(SELECTING):
		{
			if (shiftSelects == true)
			{
				App->moduleEditor->AddSelect(gameObject, false);
			}
			else
			{
				App->moduleEditor->UnselectSingle(gameObject);
			}
			if (begunSelection == false)
			{
				if (gameObject == App->moduleEditor->lastSelected || gameObject == shiftClickedGO)
				{
					App->moduleEditor->lastSelected = shiftClickedGO;
					shiftSelectionStage = NONE;
				}
			}

		}
	}
}

GameObject* P_Hierarchy::GetPreviousHierarchyNode(GameObject* gameObject) const
{
	GameObject* ret = nullptr;
	int childIndex = gameObject->parent->GetChildIndex(gameObject);
	if (childIndex > 0)
	{
		GameObject* previousSibling = gameObject->parent->GetChild(--childIndex);
		while (previousSibling->hierarchyOpen == true)
		{
			previousSibling = previousSibling->GetChild(previousSibling->childs.size() - 1);
		}
		return previousSibling;
	}
	else
	{
		return gameObject->parent;
	}
}

GameObject* P_Hierarchy::GetNextHierarchyNode(GameObject* gameObject) const
{
	GameObject* ret = nullptr;
	GameObject* parent = gameObject->parent;

	if (gameObject->childs.size() > 0  && gameObject->hierarchyOpen == true)
		return gameObject->childs[0];

	while (parent != nullptr)
	{
		if (parent->hierarchyOpen == true)
		{
			int childIndex = parent->GetChildIndex(gameObject);
			//Returning next "sibling" if exists and is open
			if (ret = parent->GetChild(++childIndex))
				return ret;
		}
		gameObject = parent;
		parent = parent->parent;
	}
	return nullptr;
}



void P_Hierarchy::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.60) - position.y;

}