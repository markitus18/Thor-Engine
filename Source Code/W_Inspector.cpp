#include "W_Inspector.h"

#include "ImGui/imgui.h"

#include "M_Editor.h"
#include "Application.h"
#include "M_Input.h"
#include "M_Scene.h"
#include "M_Resources.h"
#include "M_Camera3D.h"
#include "M_Renderer3D.h"

#include "TreeNode.h"
#include "GameObject.h"

#include "C_Mesh.h"
#include "C_Transform.h" //TODO: why wasnt it necessary?
#include "C_Material.h"
#include "C_Bone.h"
#include "C_Animation.h"
#include "C_Camera.h"
#include "C_Billboard.h"

#include "R_Material.h"
#include "R_Texture.h"
#include "R_Animation.h"

W_Inspector::W_Inspector(M_Editor* editor) : DWindow(editor, "Inspector")
{
	billboardAlignmentOptions.push_back("Screen");
	billboardAlignmentOptions.push_back("Camera");
	billboardAlignmentOptions.push_back("Axis");

	billboardLockOptions.push_back("x");
	billboardLockOptions.push_back("y");
	billboardLockOptions.push_back("z");

}

void W_Inspector::Draw()
{
	if (editor->selectedGameObjects.size() > 0)
	{
		DrawGameObject((GameObject*)editor->selectedGameObjects[0]);
	}

	
}

void W_Inspector::OnResize()
{

}

void W_Inspector::DrawGameObject(GameObject* gameObject)
{
	ImGui::Indent();

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

	ImGui::Unindent();

	ImGui::Separator();
	ImGui::Separator();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	DrawTransform(gameObject, transform);

	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	DrawMesh(gameObject, mesh);

	C_Camera* camera = gameObject->GetComponent<C_Camera>();
	DrawCamera(gameObject, camera);

	C_Animation* animation = gameObject->GetComponent<C_Animation>();
	DrawAnimation(gameObject, animation);

	C_Bone* bone = gameObject->GetComponent<C_Bone>();
	DrawBone(gameObject, bone);

	C_Billboard* billboard = gameObject->GetComponent<C_Billboard>();
	DrawBillboard(gameObject, billboard);

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Add Component Popup");
	}

	if (ImGui::BeginPopup("Add Component Popup"))
	{
		if (ImGui::MenuItem("Mesh"))
		{
			gameObject->CreateComponent(Component::Type::Mesh);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Material"))
		{
			gameObject->CreateComponent(Component::Type::Material);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Camera"))
		{
			gameObject->CreateComponent(Component::Type::Camera);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Animation"))
		{
			gameObject->CreateComponent(Component::Type::Animation);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Billboard"))
		{
			gameObject->CreateComponent(Component::Type::Billboard);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}

void W_Inspector::DrawTransform(GameObject* gameObject, C_Transform* transform)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

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

		if (showDebug)
		{
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("Local Matrix");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					ImGui::Text("%f", transform->GetTransform()[j][i]);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::Separator();

			ImGui::Text("Global Matrix");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					ImGui::Text("%f", transform->GetGlobalTransform()[j][i]);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::Separator();
		}
		ImGui::Unindent();
	}
}

void W_Inspector::DrawMesh(GameObject* gameObject, C_Mesh* mesh)
{
	std::vector<C_Material*> materials;
	gameObject->GetComponents(materials);
	if (mesh == nullptr) return;

	R_Mesh* rMesh = (R_Mesh*)mesh->GetResource();
	if (rMesh != nullptr)
	{
		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();

			ImGui::Text("Materials");
			ImGui::Separator();
			ImGui::Text("Size: %i", materials.size());
			for (uint i = 0; i < materials.size(); i++)
			{
				R_Material* rMat = (R_Material*)materials[i]->GetResource();
				ImGui::Text("Element %i: %s", i, rMat->GetName());
			}

			ImGui::Unindent();
		}
	}


	if (materials.size() > 0)
	{
		for (uint i = 0; i < materials.size(); i++)
		{
			R_Material* rMat = (R_Material*)materials[i]->GetResource();
			DrawMaterial(gameObject, rMat);
		}
	}

	if (mesh->bones.size() > 0)
	{
		if (ImGui::CollapsingHeader("Bones", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (uint i = 0; i < mesh->bones.size(); i++)
			{
				ImGui::Text("Bone: %s", mesh->bones[i]->GetResource()->GetName());
			}
		}
	}
}

void W_Inspector::DrawMaterial(GameObject* gameObject, R_Material* material)
{
	if (material == nullptr) return;

	if (ImGui::CollapsingHeader(material->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (material->textureID != 0)
		{
			R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(material->textureID);
			if (rTex)
			{
				ImGui::Text(rTex->GetName());
				ImGui::Image((ImTextureID)rTex->buffer, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TEXTURE"))
					{
						if (payload->DataSize == sizeof(uint64))
						{
							uint64 resourceID = *(const uint64*)payload->Data;
							Resource* resource = App->moduleResources->GetResource(resourceID);

							if (resource->GetType() == Resource::Type::TEXTURE)
							{
								material->textureID = resourceID;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		float color[3] = { material->color.r, material->color.g, material->color.b };
		if (ImGui::ColorEdit3("Color", color))
		{
			material->color.Set(color[0], color[1], color[2]);
			material->needs_save = true;
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawCamera(GameObject* gameObject, C_Camera* camera)
{
	if (camera == nullptr) return;

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

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

		ImGui::Unindent();
	}
}

void W_Inspector::DrawAnimation(GameObject* gameObject, C_Animation* animation)
{
	if (animation == nullptr) return;

	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		R_Animation* rAnimation = (R_Animation*)animation->GetResource();
		if (ImGui::Checkbox("Playing", &animation->playing))
		{
			animation->SetAnimation(animation->current_animation);
		}

		ImGui::Text("Animations size: %i", animation->animations.size());
		ImGui::Separator();
		ImGui::Separator();
		for (uint i = 0; i < animation->animations.size(); i++)
		{
			ImGui::Text(animation->animations[i].name.c_str());
			ImGui::Separator();

			std::string loop_label = std::string("Loop##") + std::string(std::to_string(i));
			ImGui::Checkbox(loop_label.c_str(), &animation->animations[i].loopable);

			bool isCurrent = animation->animations[i].current;
			std::string current_label = std::string("CurrentAnimation##") + std::string(std::to_string(i));

			if (ImGui::Checkbox(current_label.c_str(), &isCurrent))
			{
				if (isCurrent == true)
				{
					animation->SetAnimation(i, 2.0f);
				}
			}

			int start_frame = animation->animations[i].start_frame;
			std::string startF_label = std::string("Start Frame##") + std::string(std::to_string(i));
			if (ImGui::InputInt(startF_label.c_str(), &start_frame))
			{
				if (start_frame >= 0)
					animation->animations[i].start_frame = start_frame;
			}

			int end_frame = animation->animations[i].end_frame;
			std::string endF_label = std::string("End Frame##") + std::string(std::to_string(i));
			if (ImGui::InputInt(endF_label.c_str(), &end_frame))
			{
				if (end_frame >= 0 && end_frame != animation->animations[i].end_frame)
				{
					//TODO: restart animation?
					animation->animations[i].end_frame = end_frame;
				}
			}

			float ticksPerSecond = animation->animations[i].ticksPerSecond;
			std::string speed_label = std::string("Speed##") + std::string(std::to_string(i));
			if (ImGui::InputFloat(speed_label.c_str(), &ticksPerSecond))
			{
				if (ticksPerSecond >= 0)
					animation->animations[i].ticksPerSecond = ticksPerSecond;
			}
			ImGui::Separator();
			ImGui::Separator();
		}

		if (ImGui::Button("Add Animation"))
		{
			animation->AddAnimation();
		}

		ImGui::Unindent();
	}
}

void W_Inspector::DrawBone(GameObject* gameObject, C_Bone* bone)
{
	if (bone == nullptr) return;

	if (ImGui::CollapsingHeader("Bone", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		ImGui::Unindent();
	}
}

void W_Inspector::DrawBillboard(GameObject* gameObject, C_Billboard* billboard)
{
	if (billboard == nullptr) return;

	if (ImGui::CollapsingHeader("Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		int currentAlignmentOption = (int)billboard->alignment;
		if (ImGui::Combo("Alignment", &currentAlignmentOption, billboardAlignmentOptions.data(), (int)C_Billboard::Alignment::Unknown))
		{
			billboard->alignment = (C_Billboard::Alignment)currentAlignmentOption;
		}
		if (billboard->alignment == C_Billboard::Alignment::Axis_Aligned)
		{
			int currentLockOption = (int)billboard->lockAxis;
			if (ImGui::Combo("Axis Lock", &currentLockOption, billboardLockOptions.data(), (int)C_Billboard::Axis::Unknown))
			{
				billboard->lockAxis = (C_Billboard::Axis)currentLockOption;
			}
		}

		ImGui::Unindent();
	}
}