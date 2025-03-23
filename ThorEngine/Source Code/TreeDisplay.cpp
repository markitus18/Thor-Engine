#include "TreeDisplay.h"

#include "TreeNode.h"
#include "Engine.h" //TODO: necessary?
#include "M_Editor.h" //TODO: necessary?
#include "M_Input.h"
#include "M_SceneManager.h"

#include "GameObject.h"

#include "C_Transform.h"

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

	if (selecting == true && Engine->input->LastClickMoved() == true)
	{
		Engine->moduleEditor->dragging = true;
	}

	if (selecting == true && Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && Engine->moduleEditor->dragging == false)
	{
		Engine->moduleEditor->FinishDrag(false, false);
		selecting = false;
	}
	bool windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	if (windowHovered == false && selecting == true)
	{
		Engine->moduleEditor->toSelectGOs.clear();
		Engine->moduleEditor->toUnselectGOs.clear();
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
			((GameObject*)node)->Destroy();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (Engine->moduleEditor->dragging == true)
	{
		//Drawing inter-GO buttons
		ImVec2 cursorPos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 4.0f);
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::PushID(node->GetID()); //TODO: should be done in tree header, we need a new id here
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x, 4);
		ImGui::InvisibleButton("Button", buttonSize);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(ImVec2(cursorPos), ImVec2(cursorPos) + ImVec2(ImGui::GetWindowSize().x, 4), ImGui::GetColorU32(ImGuiCol_HeaderActive));
			if (Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				GameObject* parent = (GameObject*)(node->hierarchyOpen == true ? node : node->GetParentNode()); //TODO
				SetParentByPlace(parent, Engine->moduleEditor->toDragGOs, node->GetNextOpenNode()); //TODO
				Engine->moduleEditor->FinishDrag(true, true);
				Engine->moduleEditor->dragging = false;
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

	if (Engine->moduleEditor->dragging == true)
		flags |= ImGuiTreeNodeFlags_Fill;

	if (node->IsNodeActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));

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
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly) && Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && Engine->moduleEditor->dragging == true)
	{
		if (Engine->moduleEditor->toDragGOs.size() > 0)
		{
			bool parentSet = SetParentByPlace(node, Engine->moduleEditor->toDragGOs);
			if (parentSet)
			{
				node->beenSelected = true;
			}

			Engine->moduleEditor->FinishDrag(true, true);
			selecting = false;
		}
		Engine->moduleEditor->dragging = false;
		LOG("End dragging");
	}

	if (ImGui::IsItemHovered() && Engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		selecting = true;
		//If control is pressed, select / unselect only the GameObject
		if (Engine->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			if (node->IsSelected())
			{
				Engine->moduleEditor->UnselectSingle(node); //TODO: add to unselect
				Engine->moduleEditor->toDragGOs.push_back(node);
				Engine->moduleEditor->lastSelected = node;
			}
			else
			{
				Engine->moduleEditor->AddToSelect(node);
				Engine->moduleEditor->toDragGOs.push_back(node);
				Engine->moduleEditor->lastSelected = node;
				std::vector<TreeNode*>::iterator it;
				for (it = Engine->moduleEditor->selectedGameObjects.begin(); it != Engine->moduleEditor->selectedGameObjects.end(); ++it)
				{
					Engine->moduleEditor->toDragGOs.push_back(*it);
				}
			}
		}
		else if (Engine->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
		{
			if (Engine->moduleEditor->selectedGameObjects.size() == 0)
			{
				Engine->moduleEditor->AddToSelect(node);
				Engine->moduleEditor->toDragGOs.push_back(node);
			}
			else
			{
				DoShiftSelection(node, !node->IsSelected());
			}
			Engine->moduleEditor->lastSelected = node;
		}
		//If not ctrl pressed or shift, unselect all other GameObjects
		else
		{
			std::vector<TreeNode*>::iterator it;
			for (it = Engine->moduleEditor->selectedGameObjects.begin(); it != Engine->moduleEditor->selectedGameObjects.end(); ++it)
			{
				if (*it != node)
				{
					Engine->moduleEditor->toUnselectGOs.push_back(*it);
				}
				if (node->IsSelected() == true)
				{
					Engine->moduleEditor->toDragGOs.push_back(*it);
				}
			}

			if (node->IsSelected() == false)
			{
				Engine->moduleEditor->AddToSelect(node);
				Engine->moduleEditor->toDragGOs.push_back(node);
			}

			Engine->moduleEditor->lastSelected = node;
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
			//TODO: move to another function, this is for generic tree display
			GameObject* gameObject = (GameObject*)node;
			float3 pos = gameObject->GetComponent<C_Transform>()->GetPosition();
			
			//Engine->camera->SetNewTarget(vec(pos.x, pos.y, pos.z));
			//LOG("New camera look position: %f x, %f y, %f z", pos.x, pos.y, pos.z);
		}

	}
}

void TreeDisplay::HandleArrows()
{
	if ((Engine->input->GetKey(SDL_SCANCODE_DOWN)) == KEY_DOWN)
	{
		TreeNode* next = nullptr;
		if (Engine->moduleEditor->lastSelected != nullptr) //TODO: when removing a gameObject, set lastSelected to nullptr
		{
			next = Engine->moduleEditor->lastSelected->GetNextOpenNode();
		}
		else
		{
			next = Engine->sceneManager->GetRoot()->GetNextOpenNode();
		}
		if (next != nullptr)
		{
			if (Engine->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
				Engine->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
			{
				TreeNode* previous = Engine->moduleEditor->lastSelected->GetPreviousOpenNode();
				if (previous == nullptr || (previous != nullptr && previous->IsSelected() == true))
				{
					Engine->moduleEditor->AddSelect(next);
				}
				else if (next->IsSelected() == true)
				{
					Engine->moduleEditor->UnselectSingle(Engine->moduleEditor->lastSelected);
				}
				else
				{
					Engine->moduleEditor->AddSelect(next);
				}
			}
			else
			{
				Engine->moduleEditor->SelectSingle(next);
			}
			Engine->moduleEditor->lastSelected = next;
		}
	}

	if ((Engine->input->GetKey(SDL_SCANCODE_UP)) == KEY_DOWN)
	{
		if (Engine->moduleEditor->lastSelected != nullptr)
		{
			if (TreeNode* previous = Engine->moduleEditor->lastSelected->GetPreviousOpenNode())
			{
				if (Engine->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
					Engine->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || Engine->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
				{
					TreeNode* next = Engine->moduleEditor->lastSelected->GetNextOpenNode();
					//Bottom end of hierarchy -> next = nullptr
					if ((next != nullptr && next->IsSelected() == true))
					{
						Engine->moduleEditor->AddSelect(previous);
					}
					else if (previous->IsSelected() == true)
					{
						Engine->moduleEditor->UnselectSingle(Engine->moduleEditor->lastSelected);
					}
					else
					{
						Engine->moduleEditor->AddSelect(previous);
					}
				}
				else
				{
					Engine->moduleEditor->SelectSingle(previous);
				}
				Engine->moduleEditor->lastSelected = previous;
			}
		}

	}
}

void TreeDisplay::DoShiftSelection(TreeNode* selected, bool selects)
{
	if (Engine->moduleEditor->lastSelected != selected)
	{
		TreeNode* next = TreeNode::GetFirstOpenNode(Engine->sceneManager->GetRoot(), Engine->moduleEditor->lastSelected, selected);
		TreeNode* second = selected == next ? Engine->moduleEditor->lastSelected : selected;

		while (next != nullptr)
		{
			if (selects == true)
			{
				if (next->IsSelected() == false)
				{
					Engine->moduleEditor->AddToSelect(next);
					Engine->moduleEditor->toDragGOs.push_back(next);
				}
			}
			else if (next != selected)
			{
				Engine->moduleEditor->UnselectSingle(next);
			}
			if (next == second)
			{
				Engine->moduleEditor->lastSelected = next;
				break;
			}
			next = next->GetNextOpenNode();
		}
		for (uint i = 0; i < Engine->moduleEditor->selectedGameObjects.size(); i++)
		{
			Engine->moduleEditor->toDragGOs.push_back(Engine->moduleEditor->selectedGameObjects[i]);
		}
	}
	else
	{
		for (uint i = 0; i < Engine->moduleEditor->selectedGameObjects.size(); i++)
		{
			Engine->moduleEditor->toDragGOs.push_back(Engine->moduleEditor->selectedGameObjects[i]);
		}
	}
}

bool TreeDisplay::IsHighlighted(TreeNode* node) const
{
	if (node->IsSelected() == true)
		return true;
	std::vector<TreeNode*>::iterator it;
	for (it = Engine->moduleEditor->toSelectGOs.begin(); it != Engine->moduleEditor->toSelectGOs.end(); ++it)
	{
		if (*it == node)
			return true;
	}
	return false;
}

bool TreeDisplay::SetParentByPlace(TreeNode* parent, std::vector<TreeNode*> childs, TreeNode* next)
{
	bool ret = false;

	//Little trick, in case the parent needs to have multiple childs added
	bool previousParentHierarchyOpen = parent->hierarchyOpen;
	parent->hierarchyOpen = true;

	std::vector<TreeNode*>::const_iterator it = TreeNode::GetFirstOpenNode(Engine->sceneManager->GetRoot(), childs);
	while (it != childs.end())
	{
		if ((*it)->HasChildNodeInTree(parent) == false && next != *it && *it != parent)
		{
			(*it)->SetParentNode(parent, next);
			ret = true;
		}
		childs.erase(it);
		it = TreeNode::GetFirstOpenNode(Engine->sceneManager->GetRoot(), childs);
	}

	parent->hierarchyOpen = previousParentHierarchyOpen;
	parent->RecalculateOpenNodes();

	return ret;
}