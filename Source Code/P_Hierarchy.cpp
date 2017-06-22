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

#include "TreeNode.h"

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
		if (windowFocused == false && selecting == true)
		{

		}

		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawRootChilds(App->scene->GetRoot(), default_flags);
		ImGui::End();

		HandleArrows();

		if (selecting == true && App->input->LastClickMoved() == true)
		{
			App->moduleEditor->dragging = true;
		}

		if (selecting == true && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->moduleEditor->dragging == false)
		{
			FinishDrag(false);
			selecting = false;
		}
	}
}

void P_Hierarchy::DrawRootChilds(TreeNode* node, ImGuiTreeNodeFlags default_flags)
{
	std::vector<TreeNode*> vec = node->GetChilds();
	std::vector<TreeNode*>::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it)
	{
		DrawGameObject(*it, default_flags);
	}
}

void P_Hierarchy::DrawGameObject(TreeNode* node, ImGuiTreeNodeFlags default_flags)
{
	DisplayGameObjectNode(node, default_flags);

	HandleUserInput((GameObject*)node); //TODO

	if (ImGui::BeginPopup(node->GetName()))
	{
		if (ImGui::Button("delete"))
		{
			App->scene->DeleteGameObject((GameObject*)node); //TODO
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	
	if (App->moduleEditor->dragging == true)
	{
		//Drawwing inter-GO buttons
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.y -= 25.0f;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PushID(node->GetID()); //TODO: should be done in tree header, we need a new id here
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x, 6);
		ImGui::InvisibleButton("Button", buttonSize);
		if (ImGui::IsItemHoveredRect() && App->moduleEditor->dragging == true)
		{
			cursorPos.y += 19;
			ImGui::RenderFrame(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 6), ImGui::GetColorU32(ImGuiCol_TitleBgActive));
			ImGui::GetCurrentWindow()->DrawList->AddRect(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 6), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				GameObject* parent = (GameObject*)(node->hierarchyOpen == true ? node : node->GetParentNode()); //TODO
				SetParentByPlace(parent, App->moduleEditor->toDragGOs, node->GetNextOpenNode()); //TODO
				FinishDrag(true);
				App->moduleEditor->dragging = false;
			}
			cursorPos.y -= 19;
		}
		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.y -= 23.0f;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PopID();
	}

	if (node->hierarchyOpen)
	{
		std::vector<TreeNode*> childs = node->GetChilds();
		std::vector<TreeNode*>::iterator it;
		for (it = childs.begin(); it != childs.end(); ++it)
		{
			DrawGameObject(*it, default_flags);
		}
		if (node->hierarchyOpen)
			ImGui::TreePop();
	}
}

void P_Hierarchy::DisplayGameObjectNode(TreeNode* node, ImGuiTreeNodeFlags defaultFlags)
{
	ImGuiTreeNodeFlags gameObject_flag = defaultFlags;
	if (node->GetChilds().empty())
	{
		gameObject_flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (IsHighlighted((GameObject*)node)) //TODO
		gameObject_flag |= ImGuiTreeNodeFlags_Selected;

	if (App->moduleEditor->dragging == true)
		gameObject_flag |= ImGuiTreeNodeFlags_Fill;

	if (node->IsNodeActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.4));

	if (node->beenSelected == true)
	{
		ImGui::SetNextTreeNodeOpen(true);
		node->beenSelected = false;
	}

	bool nodeOpen = ImGui::TreeNodeEx(node, gameObject_flag, node->GetName());
	node->hierarchyOpen = node->GetChilds().empty() ? false : nodeOpen;

	if (node->IsNodeActive() == false)
		ImGui::PopStyleColor();
}

void P_Hierarchy::HandleUserInput(TreeNode* node)
{
	if (ImGui::IsItemHoveredRect() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->moduleEditor->dragging == true)
	{
		if (App->moduleEditor->toDragGOs.size() > 0)
		{
			bool parentSet = SetParentByPlace(node, App->moduleEditor->toDragGOs, nullptr);
			if (parentSet)
				node->beenSelected = true;

			FinishDrag(true);
			selecting = false;
		}
		App->moduleEditor->dragging = false;
		LOG("End dragging");
	}

	if (ImGui::IsItemHovered() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		selecting = true;
		//If control is pressed, select / unselect only the GameObject
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			if (node->IsSelected())
			{
				App->moduleEditor->UnselectSingle(node); //TODO: add to unselect
				App->moduleEditor->toDragGOs.push_back(node);
				App->moduleEditor->lastSelected = node;
			}
			else
			{
				App->moduleEditor->AddToSelect(node);
				App->moduleEditor->toDragGOs.push_back(node);
				App->moduleEditor->lastSelected = node;
				std::vector<TreeNode*>::iterator it;
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
				App->moduleEditor->AddToSelect(node);
				App->moduleEditor->toDragGOs.push_back(node);
			}
			else
			{
				DoShiftSelection(node, !node->IsSelected());
			}
			App->moduleEditor->lastSelected = node;
		}
		//If not ctrl pressed or shift, unselect all other GameObjects
		else
		{
			std::vector<TreeNode*>::iterator it;
			for (it = App->moduleEditor->selectedGameObjects.begin(); it != App->moduleEditor->selectedGameObjects.end(); ++it)
			{
				if (*it != node)
					App->moduleEditor->toUnselectGOs.push_back(*it);
				if (node->IsSelected() == true)
					App->moduleEditor->toDragGOs.push_back(*it);
			}
			if (node->IsSelected() == false)
			{
				App->moduleEditor->AddToSelect(node);
				App->moduleEditor->toDragGOs.push_back(node);
			}
			App->moduleEditor->lastSelected = node;
		}
	}

	if (ImGui::IsItemHovered())
	{
		if (ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup(node->GetName());
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			//float3 pos = gameObject->GetComponent<C_Transform>()->GetGlobalPosition(); //TODO: move to another function
			//App->camera->SetNewTarget(vec(pos.x, pos.y, pos.z));
			//LOG("New camera look position: %f x, %f y, %f z", pos.x, pos.y, pos.z);
		}

	}
}

void P_Hierarchy::HandleArrows()
{
	if ((App->input->GetKey(SDL_SCANCODE_DOWN)) == KEY_DOWN)
	{
		TreeNode* next = App->moduleEditor->lastSelected->GetNextOpenNode();
		if (next != nullptr && next != App->scene->GetRoot())
		{
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
				App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
			{
				TreeNode* previous = App->moduleEditor->lastSelected->GetPreviousOpenNode();
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
		TreeNode* previous = App->moduleEditor->lastSelected->GetPreviousOpenNode();
		if (previous != nullptr && previous != App->scene->GetRoot())
		{
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
				App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
			{
				TreeNode* next = App->moduleEditor->lastSelected->GetNextOpenNode();
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

void P_Hierarchy::DoShiftSelection(TreeNode* selected, bool selects)
{
	if (App->moduleEditor->lastSelected != selected)
	{
		TreeNode* next = TreeNode::GetFirstOpenNode(App->scene->GetRoot(), App->moduleEditor->lastSelected, selected);
		TreeNode* second = selected == next ? App->moduleEditor->lastSelected : selected;

		while (next != nullptr)
		{
			if (selects == true)
			{
				if (next->IsSelected() == false)
				{
					App->moduleEditor->AddToSelect(next);
					App->moduleEditor->toDragGOs.push_back(next);
				}
			}
			else if (next != selected)
			{
				App->moduleEditor->UnselectSingle(next);
			}
			if (next == second)
			{
				App->moduleEditor->lastSelected = next;
				break;
			}
			next = next->GetNextOpenNode();
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

void P_Hierarchy::FinishDrag(bool drag)
{
	std::vector<TreeNode*>::const_iterator it;
	for (it = App->moduleEditor->toUnselectGOs.begin(); it != App->moduleEditor->toUnselectGOs.end(); it++)
	{
		App->moduleEditor->UnselectSingle(*it);
	}
	if (drag == true)
	{
		for (it = App->moduleEditor->toDragGOs.begin(); it != App->moduleEditor->toDragGOs.end(); it++)
		{
			App->moduleEditor->AddSelect(*it);
		}
	}
	else
	{
		for (it = App->moduleEditor->toSelectGOs.begin(); it != App->moduleEditor->toSelectGOs.end(); it++)
		{
			App->moduleEditor->AddSelect(*it);
		}
	}

	App->moduleEditor->toDragGOs.clear();
	App->moduleEditor->toSelectGOs.clear();
	App->moduleEditor->toUnselectGOs.clear();
}

bool P_Hierarchy::IsHighlighted(TreeNode* node) const
{
	if (node->IsSelected() == true)
		return true;
	std::vector<TreeNode*>::iterator it;
	for (it = App->moduleEditor->toSelectGOs.begin(); it != App->moduleEditor->toSelectGOs.end(); ++it)
	{
		if (*it == node)
			return true;
	}
	return false;
}

bool P_Hierarchy::SetParentByPlace(TreeNode* parent, std::vector<TreeNode*> childs, TreeNode* next)
{
	bool previousParentHierarchyOpen = parent->hierarchyOpen;
	bool ret = false;
	parent->hierarchyOpen = true; //Little trick, in case the parent needs to add multiple childs
	std::vector<TreeNode*>::const_iterator it = TreeNode::GetFirstOpenNode(App->scene->GetRoot(), childs);
	while (it != childs.end())
	{
		if ((*it)->HasChildNodeInTree(parent) == false && next != *it && *it != parent)
		{
			(*it)->SetParentNode(parent, next);
			ret = true;
		}
		childs.erase(it);
		it = TreeNode::GetFirstOpenNode(App->scene->GetRoot(), childs);
	}
	parent->hierarchyOpen = previousParentHierarchyOpen;
	parent->RecalculateOpenNodes();
	return ret;
}

void P_Hierarchy::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.60) - position.y;

}