#include "PanelInspector.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "OpenGL.h"
#include "C_Camera.h"

PanelInspector::PanelInspector()
{
}

PanelInspector::~PanelInspector()
{
	
}

void PanelInspector::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

		if (!ImGui::Begin("Inspector", &active, ImVec2(size.x, size.y), 1.0f, flags))
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
			if (ImGui::CollapsingHeader("Transform", transform_header_flags))
			{
				if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
				{
					ImGui::OpenPopup("reset");
				}

				if (ImGui::BeginPopup("reset"))
				{
					if (ImGui::Button("Reset"))
					{
						gameObject->ResetTransform();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				float3 pos = gameObject->GetPosition();
				float3 scale = gameObject->GetScale();
				float3 rotation = gameObject->GetEulerRotation();
				if (ImGui::DragFloat3("Position", (float*)&pos, 0.15f))
				{
					App->input->InfiniteHorizontal();
					gameObject->SetPosition(pos);
				}

				if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.5f))
				{
					App->input->InfiniteHorizontal();
					gameObject->SetRotation(rotation);
				}

				if (ImGui::DragFloat3("Scale", (float*)&scale, 0.15f))
				{
					App->input->InfiniteHorizontal();
					gameObject->SetScale(scale);
				}
			}
			std::vector<Component*> mesh_vector;
			gameObject->GetComponents(Component::Type::Mesh, mesh_vector);
			C_Mesh* mesh = NULL;
			if (!mesh_vector.empty())
				mesh = (C_Mesh*)mesh_vector[0];

			if (mesh)
			{
				uint mat_size = mesh->GetMaterialsSize();
				if (ImGui::CollapsingHeader("Mesh", transform_header_flags))
				{
					ImGui::Text("Materials");
					ImGui::Separator();
					ImGui::Text("Size: %i", mat_size);
					for (uint i = 0; i < mat_size; i++)
					{
						ImGui::Text("Element %i: %s", i, mesh->GetMaterial(i)->texture_file.c_str());
					}
				}

				if (mat_size > 0)
				{
					for (uint i = 0; i < mat_size; i++)
					{
						const C_Material* mat = mesh->GetMaterial(i);
						if (ImGui::CollapsingHeader(mat->texture_file.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::Image((ImTextureID)mat->texture_id, ImVec2(128, 128));
						}
					}
				}
			}

			std::vector<Component*> camera_vector;
			gameObject->GetComponents(Component::Type::Camera, camera_vector);
			if (!camera_vector.empty())
			{
				C_Camera* camera = (C_Camera*)camera_vector[0];
				if (ImGui::CollapsingHeader("Camera", transform_header_flags))
				{
					//TODO: move this into private, more polite way?
					float3 camera_pos = camera->frustum.Pos();
					if (ImGui::DragFloat3("##Position", (float*)&camera_pos))
					{
						camera->frustum.SetPos(camera_pos);
					}
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

void PanelInspector::UpdatePosition(int screen_width, int screen_height)
{
	position.x = screen_width * (0.80);
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y =screen_height * (0.60) - position.y;
}