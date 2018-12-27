#include "TreeDisplay.h"

#include "TreeNode.h"
#include "Application.h" //TODO: necessary?
#include "M_Editor.h" //TODO: necessary?
#include "M_Input.h"
#include "M_Scene.h"
#include "GameObject.h"

#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

TreeDisplay::TreeDisplay()
{

}

TreeDisplay::~TreeDisplay()
{

}

void TreeDisplay::DrawTree(TreeNode* root)
{
	DrawNodeChilds(root);
	HandleArrows();
	if (selecting == true && App->input->LastClickMoved() == true)
	{
		App->moduleEditor->dragging = true;
	}

	if (selecting == true && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->moduleEditor->dragging == false)
	{
		App->moduleEditor->FinishDrag(false, false);
		selecting = false;
	}
	bool windowHovered = ImGui::IsWindowHovered();
	if (windowHovered == false && selecting == true)
	{
		App->moduleEditor->toSelectGOs.clear();
		App->moduleEditor->toUnselectGOs.clear();
	}
}

void TreeDisplay::DrawNode(TreeNode* node)
{
	ShowNode(node);
	HandleUserInput(node);

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
		//Drawing inter-GO buttons
		ImVec2 cursorPos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 4.0f);
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::PushID(node->GetID()); //TODO: should be done in tree header, we need a new id here
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x, 4);
		ImGui::InvisibleButton("Button", buttonSize);
		if (ImGui::IsItemHoveredRect())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 4), ImGui::GetColorU32(ImGuiCol_TitleBgActive));
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				GameObject* parent = (GameObject*)(node->hierarchyOpen == true ? node : node->GetParentNode()); //TODO
				SetParentByPlace(parent, App->moduleEditor->toDragGOs, node->GetNextOpenNode()); //TODO
				App->moduleEditor->FinishDrag(true, true);
				App->moduleEditor->dragging = false;
			}
		}
		ImGui::PopID();
		ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() - ImVec2( 0.0f, 4.0f));
	}

	if (node->hierarchyOpen == true)
	{
		DrawNodeChilds(node);
		ImGui::TreePop();
	}

}

void TreeDisplay::ShowNode(TreeNode* node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (node->GetChilds().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (IsHighlighted(node)) //TODO
		flags |= ImGuiTreeNodeFlags_Selected;

	if (App->moduleEditor->dragging == true)
		flags |= ImGuiTreeNodeFlags_Fill;

	if (node->IsNodeActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.4));

	if (node->beenSelected == true)
	{
		ImGui::SetNextTreeNodeOpen(true);
		node->beenSelected = false;
	}

	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->GetName());
	node->hierarchyOpen = node->GetChilds().empty() ? false : nodeOpen;

	if (node->IsNodeActive() == false)
		ImGui::PopStyleColor();
}

void TreeDisplay::DrawNodeChilds(TreeNode* node)
{
	std::vector<TreeNode*> vec = node->GetChilds();
	std::vector<TreeNode*>::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it)
	{
		DrawNode(*it);
	}
}

void TreeDisplay::HandleUserInput(TreeNode* node)
{
	if (ImGui::IsItemHoveredRect() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->moduleEditor->dragging == true)
	{
		if (App->moduleEditor->toDragGOs.size() > 0)
		{
			bool parentSet = SetParentByPlace(node, App->moduleEditor->toDragGOs);
			if (parentSet)
				node->beenSelected = true;

			App->moduleEditor->FinishDrag(true, true);
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

void TreeDisplay::HandleArrows()
{
	if ((App->input->GetKey(SDL_SCANCODE_DOWN)) == KEY_DOWN)
	{
		TreeNode* next = nullptr;
		if (App->moduleEditor->lastSelected != nullptr) //TODO: when removing a gameObject, set lastSelected to nullptr
		{
			next = App->moduleEditor->lastSelected->GetNextOpenNode();
		}
		else
		{
			next = App->scene->GetRoot()->GetNextOpenNode();
		}
		if (next != nullptr)
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
		if (App->moduleEditor->lastSelected != nullptr)
		{
			if (TreeNode* previous = App->moduleEditor->lastSelected->GetPreviousOpenNode())
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
}

void TreeDisplay::DoShiftSelection(TreeNode* selected, bool selects)
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

bool TreeDisplay::IsHighlighted(TreeNode* node) const
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

bool TreeDisplay::SetParentByPlace(TreeNode* parent, std::vector<TreeNode*> childs, TreeNode* next)
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