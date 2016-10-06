#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

PanelHierarchy::PanelHierarchy()
{
	size.x = 1200;
	size.y = 200;
}

PanelHierarchy::~PanelHierarchy()
{

}

void PanelHierarchy::Draw()
{
	if (active)
	{
		ImGui::Begin("Hierarchy", &active, ImVec2(size.x, size.y), 1.0f);
		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawGameObject(App->scene->getRoot(), default_flags);
		//ImGui::Text("v0.2-alpha");
		//ImGui::Separator();
		//ImGui::Text("By Marc Garrigo for educational purposes.");
		//ImGui::Text("Thor Engine is licensed under Public Domain, see LICENSE for more information.");
		ImGui::End();
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

	bool nodeOpen = ImGui::TreeNodeEx(gameObject, gameObject_flag, gameObject->name.c_str());
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
	if (nodeOpen)
	{
		for (uint i = 0; i < gameObject->childs.size(); i++)
		{
			DrawGameObject(gameObject->childs[i], default_flags);
		}
		if (!gameObject->childs.empty())
			ImGui::TreePop();
	}
}
void PanelHierarchy::SelectSingle(GameObject* gameObject)
{
	if (!selectedGameObjects.empty())
	{
		UnselectAll();
	}
	gameObject->Select();
	selectedGameObjects.push_back(gameObject);
}

void PanelHierarchy::AddSelect(GameObject* gameObject)
{
	gameObject->Select();
	selectedGameObjects.push_back(gameObject);
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