#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "M_Camera3D.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Mesh.h"	
#include "C_Material.h"

#include "ImGui\imgui.h"

PanelHierarchy::PanelHierarchy()
{
}

PanelHierarchy::~PanelHierarchy()
{

}

void PanelHierarchy::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

		ImGui::Begin("Hierarchy", &active, ImVec2(size.x, size.y), 1.0f, flags);
		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawRootChilds(App->scene->GetRoot(), default_flags);
		ImGui::End();
	}
}

void PanelHierarchy::DrawRootChilds(GameObject* gameObject, ImGuiTreeNodeFlags default_flags)
{
	for (uint i = 0; i < gameObject->childs.size(); i++)
	{
		DrawGameObject(gameObject->childs[i], default_flags);
	}
}

void PanelHierarchy::DrawGameObject(GameObject* gameObject, ImGuiTreeNodeFlags default_flags)
{
	ImGuiTreeNodeFlags gameObject_flag = default_flags;
	if (gameObject->childs.empty())
	{
		gameObject_flag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (gameObject->IsSelected())
		gameObject_flag |= ImGuiTreeNodeFlags_Selected;

	if (gameObject->IsParentActive() == false)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.4));

	if (gameObject->beenSelected == true)
	{
		ImGui::SetNextTreeNodeOpen(true);
		LOG("Opening next tree node");
	}

	bool nodeOpen = ImGui::TreeNodeEx(gameObject, gameObject_flag, gameObject->name.c_str());
	gameObject->hierarchyOpen = gameObject->childs.empty() ? false : nodeOpen;
	//bool openAndChilds = (gameObject->hierarchyOpen && !gameObject->childs.empty());

	if (gameObject->IsParentActive() == false)
		ImGui::PopStyleColor();

	if (ImGui::IsItemClicked())
	{
		//If control is pressed, select / unselect only the GameObject
		if (ImGui::GetIO().KeyCtrl)
		{
			if (gameObject->IsSelected())
			{
				UnselectSingle(gameObject);
			}
			else
				AddSelect(gameObject);
		}
		//If not ctrl pressed, unselect all other GameObjects
		else
		{
			SelectSingle(gameObject);
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

void PanelHierarchy::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.75) - position.y;

}

void PanelHierarchy::SelectSingle(GameObject* gameObject)
{
	if (!selectedGameObjects.empty())
	{
		UnselectAll();
	}
	if (gameObject)
	{
		gameObject->Select();
		selectedGameObjects.push_back(gameObject);

		GameObject* it = gameObject->parent;
		while (it != nullptr)
		{
			it->beenSelected = true;
			it = it->parent;
		}
	}
}

void PanelHierarchy::AddSelect(GameObject* gameObject)
{
	if (gameObject)
	{
		gameObject->Select();
		selectedGameObjects.push_back(gameObject);

		GameObject* it = gameObject->parent;
		while (it != nullptr && it->name != "root")
		{
			it->beenSelected = true;
			it = it->parent;
		}
	}

}

void PanelHierarchy::UnselectSingle(GameObject* gameObject)
{
	gameObject->Unselect();
	std::vector<GameObject*>::iterator it = selectedGameObjects.begin();
	while (it != selectedGameObjects.end())
		if ((*it) == gameObject)
		{
			selectedGameObjects.erase(it);
			break;
		}
}

void PanelHierarchy::UnselectAll()
{
	for (uint i = 0; i < selectedGameObjects.size(); i++)
	{
		selectedGameObjects[i]->Unselect();
	}
	selectedGameObjects.clear();
}