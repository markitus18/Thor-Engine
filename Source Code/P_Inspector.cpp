#include "P_Inspector.h"

#include "Application.h"
#include "M_Editor.h"

#include "GameObject.h"
#include "M_Input.h"
#include "OpenGL.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Bone.h"

#include "R_Mesh.h"
#include "R_Material.h"
#include "R_Texture.h"
#include "R_Animation.h"

#include "M_Renderer3D.h"
#include "M_Camera3D.h"

#include "M_Scene.h"
#include "Quadtree.h"
#include "M_Resources.h"

P_Inspector::P_Inspector()
{
}

P_Inspector::~P_Inspector()
{
	
}

void P_Inspector::Draw(ImGuiWindowFlags flags)
{
	if (active)
	{
		ImGui::SetNextWindowPos(position);
		ImGui::SetNextWindowSize(size);

		if (!ImGui::Begin("Inspector", &active, ImVec2(size), 1.0f, flags))
		{
			ImGui::End();
			return;
		}

		if (App->moduleEditor->selectedGameObjects.size() == 1)
		{
			GameObject* gameObject = App->moduleEditor->selectedGameObjects[0];
			DrawGameObject(gameObject);
		}
		ImGui::End();
	}
}

void P_Inspector::DrawGameObject(GameObject* gameObject)
{
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

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	DrawTransform(gameObject, transform, flags);

	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	DrawMesh(gameObject, mesh, flags);

	C_Camera* camera = gameObject->GetComponent<C_Camera>();
	DrawCamera(gameObject, camera, flags);

	C_Animation* animation = gameObject->GetComponent<C_Animation>();
	DrawAnimation(gameObject, animation, flags);

	C_Bone* bone = gameObject->GetComponent<C_Bone>();
	DrawBone(gameObject, bone, flags);
}

void P_Inspector::DrawTransform(GameObject* gameObject, C_Transform* transform, ImGuiWindowFlags flags)
{
	if (ImGui::CollapsingHeader("Transform", flags))
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
}

void P_Inspector::DrawMesh(GameObject* gameObject, C_Mesh* mesh, ImGuiWindowFlags flags)
{
	std::vector<C_Material*> materials;
	gameObject->GetComponents(materials);
	if (mesh != nullptr)
	{
		R_Mesh* rMesh = (R_Mesh*)mesh->GetResource();
		if (rMesh != nullptr)
		{
			if (ImGui::CollapsingHeader("Mesh", flags))
			{
				ImGui::Text("Materials");
				ImGui::Separator();
				ImGui::Text("Size: %i", materials.size());
				for (uint i = 0; i < materials.size(); i++)
				{
					R_Material* rMat = (R_Material*)materials[i]->GetResource();
					ImGui::Text("Element %i: %s", i, rMat->GetName());
				}
			}
		}


		if (materials.size() > 0)
		{
			for (uint i = 0; i < materials.size(); i++)
			{
				R_Material* rMat = (R_Material*)materials[i]->GetResource();
				DrawMaterial(gameObject, rMat, flags);
			}
		}

		if (mesh->bones.size() > 0)
		{
			if (ImGui::CollapsingHeader("Bones"), flags)
			{
				for (uint i = 0; i < mesh->bones.size(); i++)
				{
					ImGui::Text("Bone: %s", mesh->bones[i]->GetResource()->GetName());
				}
			}
		}
	}
}

void P_Inspector::DrawMaterial(GameObject* gameObject, R_Material* material, ImGuiWindowFlags flags)
{
	if (ImGui::CollapsingHeader(material->GetName(), flags))
	{
		if (material->textureID != 0)
		{
			R_Texture* rTex = (R_Texture*)App->moduleResources->GetResource(material->textureID);
			if (rTex)
			{
				ImGui::Text(rTex->GetName());
				ImGui::Image((ImTextureID)rTex->buffer, ImVec2(128, 128));
			}
		}

		float color[3] = { material->color.r, material->color.g, material->color.b };
		if (ImGui::ColorEdit3("Color", color))
		{
			material->color.Set(color[0], color[1], color[2]);
			material->needs_save = true;
		}
	}
}

void P_Inspector::DrawCamera(GameObject* gameObject, C_Camera* camera, ImGuiWindowFlags flags)
{
	if (camera != nullptr)
	{
		if (ImGui::CollapsingHeader("Camera", flags))
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

void P_Inspector::DrawAnimation(GameObject* gameObject, C_Animation* animation, ImGuiWindowFlags flags)
{
	if (animation)
	{
		if (ImGui::CollapsingHeader("Animation", flags))
		{
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
		}
	}
}

void P_Inspector::DrawBone(GameObject* gameObject, C_Bone* bone, ImGuiWindowFlags flags)
{
	if (bone)
	{
		if (ImGui::CollapsingHeader("Bone", flags))
		{

		}
	}
}

void P_Inspector::UpdatePosition(int screen_width, int screen_height)
{
	position.x = screen_width * (0.80);
	position.y = 19;
	size.x = screen_width * (0.20);
	size.y = screen_height * (0.60) - position.y;
}