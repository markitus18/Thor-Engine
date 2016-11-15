#include "PanelInspector.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "OpenGL.h"
#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"

#include "ModuleScene.h"
#include "Quadtree.h"

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
			ImGui::SameLine();
			bool gameObject_static = gameObject->isStatic;
			if (ImGui::Checkbox("static", &gameObject_static))
			{
				App->scene->SetStaticGameObject(gameObject, gameObject_static, true);
			}
			ImGui::Separator();
			ImGui::Separator();

			ImGuiTreeNodeFlags transform_header_flags = ImGuiTreeNodeFlags_DefaultOpen;
			C_Transform* transform = gameObject->GetComponent<C_Transform>();
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
						transform->Reset();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				float3 pos = transform->GetPosition();
				float3 scale = transform->GetScale();
				float3 rotation = transform->GetEulerRotation();

				if (ImGui::DragFloat3("Position", (float*)&pos, 0.15f))
				{
					App->input->InfiniteHorizontal();
					transform->SetPosition(pos);
				}

				if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.5f))
				{
					App->input->InfiniteHorizontal();
					transform->SetEulerRotation(rotation);
				}

				if (ImGui::DragFloat3("Scale", (float*)&scale, 0.15f))
				{
					App->input->InfiniteHorizontal();
					transform->SetScale(scale);
				}
			}
			
			C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
			std::vector<C_Material*> materials;
			gameObject->GetComponents(materials);
			if (mesh)
			{
				if (ImGui::CollapsingHeader("Mesh", transform_header_flags))
				{
					ImGui::Text("Materials");
					ImGui::Separator();
					ImGui::Text("Size: %i", materials.size());
					for (uint i = 0; i < materials.size(); i++)
					{
						ImGui::Text("Element %i: %s", i, materials[i]->libFile.c_str());
					}
				}

				if (materials.size() > 0)
				{
					for (uint i = 0; i < materials.size(); i++)
					{
						if (ImGui::CollapsingHeader(materials[i]->libFile.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::Text(materials[i]->texture_file.c_str());
							ImGui::Image((ImTextureID)materials[i]->texture_id, ImVec2(128, 128));
						}
					}
				}
			}

			C_Camera* camera = gameObject->GetComponent<C_Camera>();
			if (camera)
			{
				if (ImGui::CollapsingHeader("Camera", transform_header_flags))
				{
					if (ImGui::Button("Match camera"))
					{
						App->camera->Match(camera);
					}

					if (ImGui::Checkbox("Set View Camera", &camera->active_camera))
					{
						camera->active_camera ? App->renderer3D->SetActiveCamera(camera) : App->renderer3D->SetActiveCamera(nullptr);
					}

					if (ImGui::Checkbox("Camera Culling", &camera->culling))
					{
						camera->culling ? App->renderer3D->SetCullingCamera(camera) : App->renderer3D->SetCullingCamera(nullptr);
					}

					//TODO: move all frustum functions to C_Camera and call from there
					float camera_fov = camera->GetFOV();
					if (ImGui::DragFloat("Field of View", (float*)&camera_fov))
					{
						camera->SetFOV(camera_fov);
					}

					float camera_near_plane = camera->GetNearPlane();
					if (ImGui::DragFloat("Near plane", &camera_near_plane))
					{
						camera->SetNearPlane(camera_near_plane);
					}

					float camera_far_plane = camera->GetFarPlane();
					if (ImGui::DragFloat("Far plane", &camera_far_plane))
					{
						camera->SetFarPlane(camera_far_plane);
					}
				}
			}
		}
		ImGui::End();
	}
}

void PanelInspector::UpdatePosition(int screen_width, int screen_height)
{
	position.x = screen_width * (0.80);
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y =screen_height * (0.60) - position.y;
}