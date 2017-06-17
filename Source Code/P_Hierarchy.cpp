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
		if (windowFocused == false && selectionType != Selection_Type::NONE)
		{

		}

		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawRootChilds(App->scene->GetRoot(), default_flags);
		ImGui::End();

		switch (shiftSelectionStage)
		{
			case(ShiftStages::SELECT_START):
			{
				DoShiftSelection();
				shiftSelectionStage = ShiftStages::NONE;
				break;
			}
		}

		HandleArrows();

		//Start the dragging party!
		if (selectionType != Selection_Type::NONE && App->input->LastClickMoved() == true)
		{
			dragging = true;
		}

		if (selectionType != Selection_Type::NONE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && dragging == false)
		{
			switch (selectionType)
			{
				case(Selection_Type::NORMAL):
				{
					if (App->moduleEditor->toSelectGOs.size() > 0)
						App->moduleEditor->SelectSingle(App->moduleEditor->toSelectGOs[0]);
					break;
				}
				case(Selection_Type::SHIFT):
				{
					for (uint i = 0; i < App->moduleEditor->toSelectGOs.size(); i++)
					{
						App->moduleEditor->AddSelect(App->moduleEditor->toSelectGOs[i]);
					}
					App->moduleEditor->lastSelected = shiftClickedGO;
					break;
				}
				case (Selection_Type::CTRL_PLUS):
				{
					App->moduleEditor->AddSelect(App->moduleEditor->toSelectGOs[0]);
					break;
				}
			}
			selectionType = Selection_Type::NONE;
			App->moduleEditor->toDragGOs.clear();
			App->moduleEditor->toSelectGOs.clear();
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

	if (shiftSelectionStage != ShiftStages::NONE)
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
	
	if (dragging == true)
	{
		//Drawwing inter-GO buttons
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.y -= 25.0f;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PushID(gameObject->uid); //TODO: should be done in tree header, we need a new id here
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x, 6);
		ImGui::InvisibleButton("Button", buttonSize);
		if (ImGui::IsItemHoveredRect() && dragging == true)
		{
			cursorPos.y += 19;
			ImGui::RenderFrame(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 6), ImGui::GetColorU32(ImGuiCol_TitleBgActive));
			ImGui::GetCurrentWindow()->DrawList->AddRect(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 6), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				GameObject* parent = gameObject->hierarchyOpen == true ? gameObject : gameObject->parent;
				SetParentByPlace(parent, App->moduleEditor->toDragGOs, GetNextHierarchyNode(gameObject));
				FinishDrag();
				dragging = false;
			}
			cursorPos.y -= 19;
		}
		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.y -= 23.0f;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PopID();
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
}

void P_Hierarchy::DisplayGameObjectNode(GameObject* gameObject, ImGuiTreeNodeFlags defaultFlags)
{
	ImGuiTreeNodeFlags gameObject_flag = defaultFlags;
	if (gameObject->childs.empty())
	{
		gameObject_flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (IsHighlighted(gameObject))
		gameObject_flag |= ImGuiTreeNodeFlags_Selected;

	if (dragging == true)
		gameObject_flag |= ImGuiTreeNodeFlags_Fill;

	if (gameObject->IsParentActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.4));

	if (gameObject->beenSelected == true)
	{
		ImGui::SetNextTreeNodeOpen(true);
		gameObject->beenSelected = false;
	}

	bool nodeOpen = ImGui::TreeNodeEx(gameObject, gameObject_flag, gameObject->name.c_str());
	gameObject->hierarchyOpen = gameObject->childs.empty() ? false : nodeOpen;

	if (gameObject->IsParentActive() == false)
		ImGui::PopStyleColor();
}

void P_Hierarchy::HandleUserInput(GameObject* gameObject)
{
	if (ImGui::IsItemHoveredRect() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && dragging == true)
	{
		if (App->moduleEditor->toDragGOs.size() > 0)
		{
			if (App->moduleEditor->toDragGOs.size() > 0)
				gameObject->beenSelected = true;

			SetParentByPlace(gameObject, App->moduleEditor->toDragGOs, nullptr);
			FinishDrag();
			selectionType = Selection_Type::NONE;
		}
		dragging = false;
		LOG("End dragging");
	}

	if (ImGui::IsItemHovered() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		//If control is pressed, select / unselect only the GameObject
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			if (gameObject->IsSelected())
			{
				App->moduleEditor->UnselectSingle(gameObject); //TODO: add to unselect
				App->moduleEditor->toDragGOs.push_back(gameObject);
				App->moduleEditor->lastSelected = gameObject;
				selectionType = Selection_Type::CTRL_MINUS;
			}
			else
			{
				App->moduleEditor->AddToSelect(gameObject);
				App->moduleEditor->toDragGOs.push_back(gameObject);
				App->moduleEditor->lastSelected = gameObject;
				selectionType = Selection_Type::CTRL_PLUS;
				std::vector<GameObject*>::iterator it;
				for (it = App->moduleEditor->selectedGameObjects.begin(); it != App->moduleEditor->selectedGameObjects.end(); ++it)
				{
					App->moduleEditor->toDragGOs.push_back(*it);
				}
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
		{
			if (App->moduleEditor->selectedGameObjects.size() == 0)
			{
				App->moduleEditor->AddToSelect(gameObject);
				App->moduleEditor->toDragGOs.push_back(gameObject);
				selectionType = Selection_Type::NORMAL;
			}
			else
			{
				shiftSelectionStage = ShiftStages::SELECT_START;
				shiftClickedGO = gameObject;
				shiftSelects = !gameObject->IsSelected();
				selectionType = Selection_Type::SHIFT;
			}
		}
		//If not ctrl pressed or shift, unselect all other GameObjects
		else
		{
			if (gameObject->IsSelected() == false)
			{
				App->moduleEditor->UnselectAll();
				App->moduleEditor->AddToSelect(gameObject);
				App->moduleEditor->toDragGOs.push_back(gameObject);
				//TODO: add to unselect
			}
			else
			{
				App->moduleEditor->AddToSelect(gameObject);
				std::vector<GameObject*>::iterator it;
				for (it = App->moduleEditor->selectedGameObjects.begin(); it != App->moduleEditor->selectedGameObjects.end(); ++it)
				{
					App->moduleEditor->toDragGOs.push_back(*it);
				}
			}
			selectionType = Selection_Type::NORMAL;
		}
	}

	if (ImGui::IsItemHovered())
	{
		if (ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup(gameObject->name.c_str());
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			float3 pos = gameObject->GetComponent<C_Transform>()->GetGlobalPosition();
			App->camera->SetNewTarget(vec(pos.x, pos.y, pos.z));
			LOG("New camera look position: %f x, %f y, %f z", pos.x, pos.y, pos.z);
		}

	}
}

void P_Hierarchy::HandleArrows()
{
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

void P_Hierarchy::HandleShiftSelection(GameObject* gameObject)
{
	bool begunSelection = false;
	switch (shiftSelectionStage)
	{
		case(ShiftStages::SELECTION_LOOP):
		{
			if (gameObject == App->moduleEditor->lastSelected || gameObject == shiftClickedGO)
			{
				bool finished = false;
				GameObject* next = gameObject;
				while (finished == false && next != nullptr)
				{
					if (shiftSelects == true)
					{
						App->moduleEditor->AddToSelect(gameObject);
						//App->moduleEditor->AddSelect(next, false);
					}
					else
					{
						App->moduleEditor->UnselectSingle(next);
					}
					next = GetNextHierarchyNode(next);
				}
			}
			shiftSelectionStage = ShiftStages::NONE;
		}
	}
}

void P_Hierarchy::DoShiftSelection()
{
	if (App->moduleEditor->lastSelected != shiftClickedGO)
	{
		GameObject* next = GetFirstHierarchyOpen(shiftClickedGO, App->moduleEditor->lastSelected);
		GameObject* second = shiftClickedGO == next ? App->moduleEditor->lastSelected : shiftClickedGO;
		while (next != nullptr)
		{
			if (shiftSelects == true)
			{
				if (next->IsSelected() == false)
				{
					App->moduleEditor->AddToSelect(next);
					App->moduleEditor->toDragGOs.push_back(next);
				}
			}
			else if (next != shiftClickedGO)
			{
				App->moduleEditor->UnselectSingle(next);
			}
			if (next == second)
			{
				App->moduleEditor->lastSelected = next;
				break;
			}
			next = GetNextHierarchyNode(next);
		}
		for (uint i = 0; i < App->moduleEditor->selectedGameObjects.size(); i++)
		{
			App->moduleEditor->toDragGOs.push_back(App->moduleEditor->selectedGameObjects[i]);
		}
	}
	else
	{
		for (uint i = 0; i < App->moduleEditor->selectedGameObjects.size(); i++)
		{
			App->moduleEditor->toDragGOs.push_back(App->moduleEditor->selectedGameObjects[i]);
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

void P_Hierarchy::FinishDrag()
{
	App->moduleEditor->toDragGOs.clear();
	std::vector<GameObject*>::const_iterator it;
	for (it = App->moduleEditor->toSelectGOs.begin(); it != App->moduleEditor->toSelectGOs.end(); it++)
	{
		App->moduleEditor->AddSelect(*it);
	}
	App->moduleEditor->toSelectGOs.clear();

	for (it = App->moduleEditor->toUnselectGOs.begin(); it != App->moduleEditor->toUnselectGOs.end(); it++)
	{
		App->moduleEditor->UnselectSingle(*it);
	}
	App->moduleEditor->toUnselectGOs.clear();
}

bool P_Hierarchy::IsHighlighted(GameObject* gameObject) const
{
	if (gameObject->IsSelected() == true)
		return true;
	std::vector<GameObject*>::iterator it;
	for (it = App->moduleEditor->toSelectGOs.begin(); it != App->moduleEditor->toSelectGOs.end(); ++it)
	{
		if (*it == gameObject)
			return true;
	}
	return false;
}

void P_Hierarchy::SetParentByPlace(GameObject* parent, std::vector<GameObject*>& childs, GameObject* next)
{
	bool previousParentHierarchyOpen = parent->hierarchyOpen;
	parent->hierarchyOpen = true; //Little trick, in case the parent needs to add multiple childs
	std::vector<GameObject*>::const_iterator it = GetFirstHierarchyOpen(childs);
	while (it != childs.end())
	{
		if ((*it)->HasChildInTree(parent) == false && next != *it)
		{
			(*it)->SetParent(parent, next);
		}
		childs.erase(it);
		it = GetFirstHierarchyOpen(childs);
	}
	parent->hierarchyOpen = previousParentHierarchyOpen;
	RecalcOpenNodes(parent);
}

void P_Hierarchy::RecalcOpenNodes(GameObject* parent)
{
	if (parent->hierarchyOpen == true)
		parent->beenSelected = true;

	std::vector<GameObject*>::const_iterator it;
	for (it = parent->childs.begin(); it != parent->childs.end(); ++it)
	{
		RecalcOpenNodes(*it);
	}
}

GameObject* P_Hierarchy::GetFirstHierarchyOpen(GameObject* first, GameObject* second) const
{
	if (first == second) return first;

	GameObject* toEvaluate = GetNextHierarchyNode(App->scene->GetRoot());
	while (toEvaluate != nullptr)
	{
		if (first == toEvaluate) return first;
		if (second == toEvaluate) return second;
		toEvaluate = GetNextHierarchyNode(toEvaluate);
	}
	return nullptr;
}

std::vector<GameObject*>::const_iterator P_Hierarchy::GetFirstHierarchyOpen(const std::vector<GameObject*>& vector) const
{
	GameObject* toEvaluate = GetNextHierarchyNode(App->scene->GetRoot());
	while (toEvaluate != nullptr)
	{
		std::vector<GameObject*>::const_iterator it;
		for (it = vector.begin(); it != vector.end(); ++it)
		{
			if (*it == toEvaluate)
				return it;
		}
		toEvaluate = GetNextHierarchyNode(toEvaluate);
	}
	return vector.end();
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