#include "PanelInspector.h"

#include "ImGui\imgui.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "GameObject.h"

PanelInspector::PanelInspector()
{
	size.x = 340;
	size.y = 380;
}

PanelInspector::~PanelInspector()
{

}

void PanelInspector::Draw()
{
	if (active)
	{
		if (!ImGui::Begin("Inspector", &active, ImVec2(size.x, size.y), 1.0f))
		{
			ImGui::End();
			return;
		}

		if (App->moduleEditor->panelHierarchy->selectedGameObjects.size() == 1)
		{
			GameObject* gameObject = App->moduleEditor->panelHierarchy->selectedGameObjects[0];

			//Active button
			ImGui::Checkbox("##", &gameObject->active);
			ImGui::SameLine();

			//Name input
			char go_name[50];
			strcpy_s(go_name, 50, gameObject->name.c_str());
			ImGuiInputTextFlags name_input_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
			if (ImGui::InputText("###", go_name, 50, name_input_flags))
				gameObject->name = go_name;
			
			ImGui::Separator();
			ImGui::Separator();

			ImGuiTreeNodeFlags transform_header_flags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader("Transform"), transform_header_flags)
			{
				float3 pos = gameObject->GetPosition();
				float3 scale = gameObject->GetScale();
				//const Quat rotation = gameObject->GetQuatRotation();
				if (ImGui::DragFloat3("Position", (float*)&pos))
				{
					gameObject->SetPosition(pos);
				}

			}
		}

		ImGui::End();
	}

	/* STYLE TODO: window swap with another window as buttons
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = style.Colors[ImGuiCol_TitleBg];
	style.WindowRounding = 0;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
	bool active0 = true;
	ImGui::Begin("Stuf", &active0, window_flags);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	ImGui::Button("Console");
	ImGui::SameLine();
	ImGui::Button("Debug console");
	ImGui::End();
	*/
}