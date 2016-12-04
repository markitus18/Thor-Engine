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

		ImGui::Begin("Hierarchy", &active, size, 1.0f, flags);
		ImGuiTreeNodeFlags default_flags =  ImGuiTreeNodeFlags_OpenOnArrow;
		DrawRootChilds(App->scene->GetRoot(), default_flags);
		ImGui::End();
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

void P_Hierarchy::UpdatePosition(int screen_width, int screen_height)
{
	position.x = 0;
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.60) - position.y;

}

void P_Hierarchy::SelectSingle(GameObject* gameObject, bool openTree)
{
	if (!selectedGameObjects.empty())
	{
		UnselectAll();
	}
	if (gameObject)
	{
		gameObject->Select();
		selectedGameObjects.push_back(gameObject);

		if (openTree)
		{
			//Opening tree hierarchy node
			GameObject* it = gameObject->parent;
			while (it != nullptr)
			{
				it->beenSelected = true;
				it = it->parent;
			}
		}
	}
}

void P_Hierarchy::AddSelect(GameObject* gameObject, bool openTree)
{
	if (gameObject)
	{
		gameObject->Select();
		selectedGameObjects.push_back(gameObject);

		if (openTree)
		{
			//Opening tree hierarchy node
			GameObject* it = gameObject->parent;
			while (it != nullptr && it->name != "root")
			{
				it->beenSelected = true;
				it = it->parent;
			}
		}
	}
}

void P_Hierarchy::UnselectSingle(GameObject* gameObject)
{
	gameObject->Unselect();
	std::vector<GameObject*>::iterator it = selectedGameObjects.begin();
	while (it != selectedGameObjects.end())
	{
		if ((*it) == gameObject)
		{
			selectedGameObjects.erase(it);
			break;
		}
		it++;
	}

}

void P_Hierarchy::UnselectAll()
{
	for (uint i = 0; i < selectedGameObjects.size(); i++)
	{
		selectedGameObjects[i]->Unselect();
	}
	selectedGameObjects.clear();
}

void P_Hierarchy::DeleteSelected()
{
	//Warning: iterator is not moved because GameObject will be erased from vector on "OnRemove" call
	for (uint i = 0; i < selectedGameObjects.size(); )
	{
		selectedGameObjects[i]->Unselect();
		App->scene->DeleteGameObject(selectedGameObjects[i]);
	}
	selectedGameObjects.clear();
}